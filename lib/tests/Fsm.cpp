#include <catch.hpp>
#include <Fsm.hpp>
#include <FsmBuilder.hpp>
#include <FsmDecorators.hpp>
#include "CsvParser.hpp"

enum class State
{
	Start,
	CommaFound,
	NewlineFound,
	End
};

TEST_CASE("Functionality", "[Fsm/FsmBuilder]")
{
	unsigned logic1_cnt = 0;
	unsigned logic2_cnt = 0;
	unsigned pred1_cnt = 0;
	unsigned pred2_cnt = 0;
	unsigned pred3_cnt = 0;

	auto logic1 = [&] (Blackboard&) { logic1_cnt++; };
	auto logic2 = [&] (Blackboard&) { logic2_cnt++; };
	auto predicate1 = [&] (const Blackboard&) { pred1_cnt++; return false; };
	auto predicate2 = [&] (const Blackboard&) { pred2_cnt++; return false; };
	auto predicate3 = [&] (const Blackboard&) { pred3_cnt++; return true; };

	Blackboard blackboard;

	SECTION("Properly executes FSM with one state and just default transitions")
	{
		auto fsm = dgm::fsm::Builder<Blackboard, State>()
			.with(State::Start)
			.exec(logic1).andLoop()
			.build();

		fsm.update(blackboard);
		fsm.update(blackboard);

		REQUIRE(logic1_cnt == 2u);
	}

	SECTION("Properly executes FSM with two states and just default transitions")
	{
		auto fsm = dgm::fsm::Builder<Blackboard, State>()
			.with(State::Start)
			.exec(logic1).andGoTo(State::End)
			.with(State::End)
			.exec(logic2).andGoTo(State::Start)
			.build();

		fsm.update(blackboard);
		fsm.update(blackboard);

		REQUIRE(logic1_cnt == 1u);
		REQUIRE(logic2_cnt == 1u);
	}

	SECTION("Properly calls predicates on a state")
	{
		auto fsm = dgm::fsm::Builder<Blackboard, State>()
			.with(State::Start)
			.when(predicate1).goTo(State::Start)
			.orWhen(predicate2).goTo(State::Start)
			.otherwiseExec(logic1).andLoop()
			.build();

		fsm.update(blackboard);

		REQUIRE(pred1_cnt == 1u);
		REQUIRE(pred2_cnt == 1u);
	}

	SECTION("Can take transitions without executing default behaviour")
	{
		auto fsm = dgm::fsm::Builder<Blackboard, State>()
			.with(State::Start)
			.when(predicate3).goTo(State::End)
			.otherwiseExec(logic1).andLoop()
			.with(State::End)
			.exec(logic2).andLoop()
			.build();

		fsm.update(blackboard);
		fsm.update(blackboard);

		REQUIRE(pred3_cnt == 1u);
		REQUIRE(logic1_cnt == 0u);
		REQUIRE(logic2_cnt == 1u);
	}
}

TEST_CASE("CanCompileBuilderSyntax", "[Fsm/FsmBuilder]")
{
	using dgm::fsm::decorator::Merge;

	auto fsm = dgm::fsm::Builder<Blackboard, State>()
		.with(State::Start)
		.when(CsvParser::isEof).goTo(State::End)
		.orWhen(CsvParser::isComma).goTo(State::CommaFound)
		.orWhen(CsvParser::isNewline).goTo(State::NewlineFound)
		.otherwiseExec(CsvParser::advanceChar).andLoop()
		.with(State::CommaFound)
		.exec(CsvParser::storeWord).andGoTo(State::Start)
		.with(State::NewlineFound)
		.exec(Merge<Blackboard>(
			CsvParser::handleNewline,
			CsvParser::storeWord)
		).andGoTo(State::Start)
		.build();
}