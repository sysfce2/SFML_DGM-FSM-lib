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
	SECTION("Properly executes FSM with one state and no transitions")
	{
		REQUIRE(false);
	}

	SECTION("Properly executes FSM with two states and just default transitions")
	{
		REQUIRE(false);
	}

	SECTION("Properly calls predicates on a state")
	{
		REQUIRE(false);
	}

	SECTION("Can take transitions without executing default behaviour")
	{
		REQUIRE(false);
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