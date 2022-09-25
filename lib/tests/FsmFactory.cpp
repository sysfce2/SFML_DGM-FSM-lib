#include <catch.hpp>
#include <FsmFactory.hpp>

using dgm::fsm::Factory;

struct Blackboard {};

class LoaderMock final : public dgm::fsm::LoaderInterface
{
public:
	dgm::fsm::loader::StateMap states;

	[[nodiscard]]
	virtual dgm::fsm::loader::StateMap loadFromFile(const std::string&) const override
	{
		return states;
	}
};

TEST_CASE("[FsmFactory]", "[FsmFactory]")
{
	auto loader = LoaderMock();
	auto factory = Factory<Blackboard>(loader);
	auto blackboard = Blackboard();

	unsigned cond1_callCnt = 0;
	unsigned cond2_callCnt = 0;
	unsigned cond3_callCnt = 0;

	unsigned logic1_callCnt = 0;
	unsigned logic2_callCnt = 0;
	unsigned logic3_callCnt = 0;

	factory.registerPredicate("cond1", [&] (const Blackboard&) -> bool
	{
		cond1_callCnt++;
		return false;
	});
	factory.registerPredicate("cond2", [&] (const Blackboard&) -> bool
	{
		cond2_callCnt++;
		return false;
	});
	factory.registerPredicate("cond3", [&] (const Blackboard&) -> bool
	{
		cond3_callCnt++;
		return false;
	});

	factory.registerLogic("logic1", [&] (Blackboard&) { logic1_callCnt++; });
	factory.registerLogic("logic2", [&] (Blackboard&) { logic2_callCnt++; });
	factory.registerLogic("logic3", [&] (Blackboard&) { logic3_callCnt++; });

	SECTION("Can construct minimal FSM with "
			"just one state and no transitions")
	{
		loader.states = {
			{0, {
				.transitions = {},
				.logic = { "logic1" },
				.defaultTransition = 0
			}}
		};

		dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		fsm.update(blackboard);

		REQUIRE(logic1_callCnt == 1u);
	}

	SECTION("Can construct minimal FSM with some transitions")
	{
		loader.states = {
			{0, {
				.transitions = {
					{ "cond1", 1 },
					{ "cond2", 1 }
				},
				.logic = { "logic1" },
				.defaultTransition = 1
			}},
			{1, {
				.transitions = {},
				.logic = { "logic2" },
				.defaultTransition = 0
			}}
		};

		dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		fsm.update(blackboard);
		fsm.update(blackboard);

		REQUIRE(cond1_callCnt == 1u);
		REQUIRE(cond2_callCnt == 1u);
		REQUIRE(logic1_callCnt == 1u);
		REQUIRE(logic2_callCnt == 1u);
	}

	SECTION("Throws if referencing not-registered logic")
	{
		loader.states = {
			{0, {
				.transitions = {},
				.logic = { "notRegisteredLogic" },
				.defaultTransition = 0
			}}
		};

		REQUIRE_THROWS([&] ()
		{
			dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		} ());
	}

	SECTION("Throws if referencing not-registered predicate")
	{
		loader.states = {
			{0, {
				.transitions = { { "notRegisteredLogic", 0 } },
				.logic = { "logic1" },
				.defaultTransition = 0
			}}
		};

		REQUIRE_THROWS([&] ()
		{
			dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		} ());
	}

	SECTION("Throws if out-of-bound state is referenced")
	{
		loader.states = {
			{0, {
				.transitions = {},
				.logic = { "logic1" },
				.defaultTransition = 1
			}}
		};

		REQUIRE_THROWS([&] ()
		{
			dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		} ());
	}
}