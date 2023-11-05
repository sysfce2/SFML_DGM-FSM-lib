#pragma once

#include <DGM/classes/Fsm.hpp>

namespace dgm
{
	namespace fsm
	{
		template<StateTypeConcept StateType, class ... BlackboardTypes>
		class Builder;

		namespace builders
		{
			template<StateTypeConcept StateType, class ... BlackboardTypes>
			class [[nodiscard]] DefaultTransitionBuilder final
			{
				Builder<StateType, BlackboardTypes...> origBuilder;
				std::vector<Transition<StateType, BlackboardTypes...>> transitions;
				Logic<BlackboardTypes...> logic;

			public:
				constexpr DefaultTransitionBuilder(
					Builder<StateType, BlackboardTypes...>&& origBuilder,
					std::vector<Transition<StateType, BlackboardTypes...>> transitions,
					Logic<BlackboardTypes...> logic)
					: origBuilder(origBuilder)
					, transitions(transitions)
					, logic(logic)
				{}

				[[nodiscard]]
				auto andGoTo(StateType state)
				{
					origBuilder.addState(
						origBuilder.getCurrentlyAddedState(),
						State<StateType, BlackboardTypes...>({
							.transitions = transitions,
							.logic = logic,
							.targetState = state
							}));
					return std::move(origBuilder);
				}

				[[nodiscard]]
				auto andLoop()
				{
					origBuilder.addState(
						origBuilder.getCurrentlyAddedState(),
						State<StateType, BlackboardTypes...>({
							.transitions = transitions,
							.logic = logic,
							.targetState = origBuilder.getCurrentlyAddedState()
							}));
					return std::move(origBuilder);
				}
			};

			// Forward declaring transition builder because there is cyclic reference between it and StateBuilder
			template<StateTypeConcept StateType, class ... BlackboardTypes>
			class TransitionBuilder;

			template<StateTypeConcept StateType, class ... BlackboardTypes>
			class [[nodiscard]] StateBuilder final
			{
				Builder<StateType, BlackboardTypes...> origBuilder;
				std::vector<Transition<StateType, BlackboardTypes...>> transitions;

			public:
				constexpr StateBuilder(
					Builder<StateType, BlackboardTypes...>&& origBuilder,
					std::vector<Transition<StateType, BlackboardTypes...>> transitions)
					: origBuilder(origBuilder)
					, transitions(transitions)
				{}

				[[nodiscard]]
				auto orWhen(Condition<BlackboardTypes...> condition)
				{
					return TransitionBuilder<StateType, BlackboardTypes...>(
						std::move(origBuilder),
						transitions,
						condition);
				}

				[[nodiscard]]
				auto otherwiseExec(Logic<BlackboardTypes...> logic)
				{
					return DefaultTransitionBuilder<StateType, BlackboardTypes...>(
						std::move(origBuilder),
						transitions,
						logic);
				}
			};

			template<StateTypeConcept StateType, class ... BlackboardTypes>
			class [[nodiscard]] TransitionBuilder final
			{
				Builder<StateType, BlackboardTypes...> origBuilder;
				std::vector<Transition<StateType, BlackboardTypes...>> transitions;
				Condition<BlackboardTypes...> condition;

			public:
				constexpr TransitionBuilder(
					Builder<StateType, BlackboardTypes...>&& origBuilder,
					std::vector<Transition<StateType, BlackboardTypes...>> transitions,
					Condition<BlackboardTypes...> condition)
					: origBuilder(origBuilder)
					, transitions(transitions)
					, condition(condition)
				{}

				[[nodiscard]]
				auto goTo(StateType state)
				{
					transitions.push_back(Transition<StateType, BlackboardTypes...>(condition, state));
					return StateBuilder<StateType, BlackboardTypes...>(
						std::move(origBuilder),
						transitions);
				}
			};

			template<StateTypeConcept StateType, class ... BlackboardTypes>
			class [[nodiscard]] EmptyStateBuilder final
			{
				Builder<StateType, BlackboardTypes...> origBuilder;

			public:
				constexpr EmptyStateBuilder(Builder<StateType, BlackboardTypes...>&& origBuilder)
					: origBuilder(origBuilder)
				{}

				[[nodiscard]]
				auto when(Condition<BlackboardTypes...> condition)
				{
					return TransitionBuilder<StateType, BlackboardTypes...>(
						std::move(origBuilder),
						{},
						condition);
				}

				[[nodiscard]]
				auto exec(Logic<BlackboardTypes...> logic)
				{
					return DefaultTransitionBuilder<StateType, BlackboardTypes...>(
						std::move(origBuilder),
						{},
						logic);
				}
			};
		}

		template<StateTypeConcept StateType, class ... BlackboardTypes>
		class [[nodiscard]] Builder final
		{
		private:
			std::map<StateType, State<StateType, BlackboardTypes...>> states;
			StateType currentlyAddedState;

		public:
			[[nodiscard]]
			auto with(StateType state)
			{
				currentlyAddedState = state;
				return builders::EmptyStateBuilder<StateType, BlackboardTypes...>(std::move(*this));
			}

			[[nodiscard]]
			constexpr StateType getCurrentlyAddedState() const noexcept
			{
				return currentlyAddedState;
			}

			void addState(StateType code, UniversalReference<State<StateType, BlackboardTypes...>> auto&& state)
			{
				states.emplace(code, std::forward<decltype(state)>(state));
			}

			[[nodiscard]]
			auto build()
			{
				return Fsm<StateType, BlackboardTypes...>(std::move(states));
			}
		};
	}
}
