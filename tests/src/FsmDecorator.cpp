#include <DGM/classes/FsmDecorators.hpp>
#include <catch2/catch_all.hpp>

struct Blackboard
{
};

bool cond1(const Blackboard&)
{
    return true;
}

bool cond2(const Blackboard&)
{
    return false;
}

int cnt = 0;

void logic1(Blackboard&)
{
    cnt++;
}

TEST_CASE("[FsmDecorators]")
{
    Blackboard board;

    SECTION("Can create conditions from decorators")
    {
        using namespace dgm::fsm::decorator;

        auto&& condAnd = And<Blackboard>(cond1, cond2, cond1, cond2);
        REQUIRE_FALSE(condAnd(board));

        auto&& condOr = Or<Blackboard>(cond1, cond2);
        REQUIRE(condOr(board));

        auto&& condNot = Not<Blackboard>(cond1);
        REQUIRE_FALSE(condNot(board));

        auto&& logic = Merge<Blackboard>(logic1, logic1, logic1);
        logic(board);
        REQUIRE(cnt == 3);
    }
}