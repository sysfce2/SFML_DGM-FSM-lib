#include <catch.hpp>
#include <Fsm.hpp>
#include <FsmBuilder.hpp>
#include <FsmDecorators.hpp>
#include "CsvParser.hpp"

enum class State {
	Start,
	CommaFound,
	NewlineFound,
	End
};

TEST_CASE("CanCompileBuilderSyntax", "Builder") {
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