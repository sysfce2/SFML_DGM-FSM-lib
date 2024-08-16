#include "CsvParser.hpp"
#include <DGM/fsm.hpp>
#include <catch2/catch_all.hpp>

enum class State
{
    Start,
    CommaFound,
    NewlineFound,
    End,
    Error
};

struct Blackboard2
{
    int value = 0;
};

void twoBbsLogic(Blackboard& bb1, Blackboard2& bb2)
{
    bb1.pos = 42;
    bb2.value = 69;
}

dgm::fsm::Fsm<State, Blackboard>
createCsvFsm(bool errorOutOnExclamationMark = false)
{
    using dgm::fsm::decorator::Merge;
    auto builder = dgm::fsm::Builder<State, Blackboard>()
                       .with(State::Start)
                       .when(CsvParser::isEof)
                       .goTo(State::End)
                       .orWhen(CsvParser::isComma)
                       .goTo(State::CommaFound)
                       .orWhen(CsvParser::isNewline)
                       .goTo(State::NewlineFound)
                       .otherwiseExec(CsvParser::advanceChar)
                       .andLoop()
                       .with(State::CommaFound)
                       .exec(CsvParser::storeWord)
                       .andGoTo(State::Start)
                       .with(State::NewlineFound)
                       .exec(Merge<Blackboard>(
                           CsvParser::handleNewline, CsvParser::storeWord))
                       .andGoTo(State::Start)
                       .with(State::Error)
                       .exec([](auto) {})
                       .andLoop();

    if (errorOutOnExclamationMark)
    {
        return builder.withGlobalErrorCondition(CsvParser::isExclamationMark)
            .goTo(State::Error)
            .build();
    }

    return builder.build();
}

std::map<State, std::string> getCsvParserLogHelper()
{
    return { { State::Start, "Start" },
             { State::CommaFound, "CommaFound" },
             { State::NewlineFound, "NewlineFound" },
             { State::End, "End" },
             { State::Error, "Error" } };
}

TEST_CASE("Functionality", "[Fsm/FsmBuilder]")
{
    unsigned logic1_cnt = 0;
    unsigned logic2_cnt = 0;
    unsigned pred1_cnt = 0;
    unsigned pred2_cnt = 0;
    unsigned pred3_cnt = 0;

    auto logic1 = [&](Blackboard&) { logic1_cnt++; };
    auto logic2 = [&](Blackboard&) { logic2_cnt++; };
    auto predicate1 = [&](const Blackboard&)
    {
        pred1_cnt++;
        return false;
    };
    auto predicate2 = [&](const Blackboard&)
    {
        pred2_cnt++;
        return false;
    };
    auto predicate3 = [&](const Blackboard&)
    {
        pred3_cnt++;
        return true;
    };

    Blackboard blackboard;

    SECTION("Properly executes FSM with one state and just default transitions")
    {
        auto fsm = dgm::fsm::Builder<State, Blackboard>()
                       .with(State::Start)
                       .exec(logic1)
                       .andLoop()
                       .build();

        fsm.update(blackboard);
        fsm.update(blackboard);

        REQUIRE(logic1_cnt == 2u);
    }

    SECTION(
        "Properly executes FSM with two states and just default transitions")
    {
        auto fsm = dgm::fsm::Builder<State, Blackboard>()
                       .with(State::Start)
                       .exec(logic1)
                       .andGoTo(State::End)
                       .with(State::End)
                       .exec(logic2)
                       .andGoTo(State::Start)
                       .build();

        fsm.update(blackboard);
        fsm.update(blackboard);

        REQUIRE(logic1_cnt == 1u);
        REQUIRE(logic2_cnt == 1u);
    }

    SECTION("Properly calls predicates on a state")
    {
        auto fsm = dgm::fsm::Builder<State, Blackboard>()
                       .with(State::Start)
                       .when(predicate1)
                       .goTo(State::Start)
                       .orWhen(predicate2)
                       .goTo(State::Start)
                       .otherwiseExec(logic1)
                       .andLoop()
                       .build();

        fsm.update(blackboard);

        REQUIRE(pred1_cnt == 1u);
        REQUIRE(pred2_cnt == 1u);
    }

    SECTION("Can create FSM with multiple blackboards")
    {
        auto fsm = dgm::fsm::Builder<State, Blackboard, Blackboard2>()
                       .with(State::Start)
                       .exec(twoBbsLogic)
                       .andLoop()
                       .build();
        fsm.setState(State::Start);

        Blackboard bb1;
        Blackboard2 bb2;
        fsm.update(bb1, bb2);

        REQUIRE(bb1.pos == 42);
        REQUIRE(bb2.value == 69);
    }

    SECTION("Can take transitions without executing default behaviour")
    {
        auto fsm = dgm::fsm::Builder<State, Blackboard>()
                       .with(State::Start)
                       .when(predicate3)
                       .goTo(State::End)
                       .otherwiseExec(logic1)
                       .andLoop()
                       .with(State::End)
                       .exec(logic2)
                       .andLoop()
                       .build();

        fsm.update(blackboard);
        fsm.update(blackboard);

        REQUIRE(pred3_cnt == 1u);
        REQUIRE(logic1_cnt == 0u);
        REQUIRE(logic2_cnt == 1u);
    }

    SECTION("Properly logs behaviour")
    {
        auto blackboard2 = Blackboard { .CSV = "a,b\nd\n" };
        std::stringstream log;

        auto fsm = createCsvFsm();
        fsm.setStateToStringHelper(getCsvParserLogHelper());
        fsm.setLogging(true, log);

        do
        {
            fsm.update(blackboard2);
        } while (fsm.getState() != State::End);

        const std::string refLog =
            R"(FSM::update(State = Start):
  behavior executed, looping
FSM::update(State = Start):
  condition hit, jumping to CommaFound
FSM::update(State = CommaFound):
  behavior executed, jumping to Start
FSM::update(State = Start):
  behavior executed, looping
FSM::update(State = Start):
  condition hit, jumping to NewlineFound
FSM::update(State = NewlineFound):
  behavior executed, jumping to Start
FSM::update(State = Start):
  behavior executed, looping
FSM::update(State = Start):
  condition hit, jumping to NewlineFound
FSM::update(State = NewlineFound):
  behavior executed, jumping to Start
FSM::update(State = Start):
  condition hit, jumping to End
)";

        REQUIRE(log.str() == refLog);
    }

    SECTION("Goes to error sink when condition is hit")
    {
        auto blackboard2 = Blackboard { .CSV = "a,b\n!d\n" };
        std::stringstream log;

        auto fsm = createCsvFsm(/* errorOutOnExclamationMark */ true);
        fsm.setStateToStringHelper(getCsvParserLogHelper());
        fsm.setLogging(true, log);

        do
        {
            fsm.update(blackboard2);
        } while (fsm.getState() != State::End
                 && fsm.getState() != State::Error);

        REQUIRE(fsm.getState() == State::Error);

        const std::string refLog =
            R"(FSM::update(State = Start):
  behavior executed, looping
FSM::update(State = Start):
  condition hit, jumping to CommaFound
FSM::update(State = CommaFound):
  behavior executed, jumping to Start
FSM::update(State = Start):
  behavior executed, looping
FSM::update(State = Start):
  condition hit, jumping to NewlineFound
FSM::update(State = NewlineFound):
  behavior executed, jumping to Start
FSM::update(State = Start):
  error condition hit, jumping to Error
)";

        REQUIRE(log.str() == refLog);
    }
}
