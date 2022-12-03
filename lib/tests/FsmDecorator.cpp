#include <catch.hpp>
#include <DGM/classes/FsmDecorators.hpp>

struct Blackboard {};

bool cond1(const Blackboard&)
{
	return true;
}

bool cond2(const Blackboard&)
{
	return false;
}

TEST_CASE("[FsmDecorators]")
{
	Blackboard board;

	SECTION("Can create conditions from decorators")
	{
		dgm::fsm::Condition<Blackboard> condAnd = dgm::fsm::decorator::And<Blackboard>(cond1, cond2);
		REQUIRE_FALSE(condAnd(board));

		dgm::fsm::Condition<Blackboard> condOr = dgm::fsm::decorator::Or<Blackboard>(cond1, cond2);
		REQUIRE(condOr(board));

		dgm::fsm::Condition<Blackboard> condNot = dgm::fsm::decorator::Not<Blackboard>(cond1);
		REQUIRE_FALSE(condNot(board));
	}
}