#pragma once

#include <map>
#include <utility>
#include <string>
#include <format>
#include <ostream>
#include <iostream>
#include <DGM/classes/FsmTypes.hpp>

namespace dgm
{
	namespace fsm
	{
		/**
		 * \brief Final state machine implementation
		 *
		 * This FSM is tailored for game AI and as such,
		 * it has some rules on how the states and transitions look like.
		 *
		 * It even has restrictions on what can be passed down to each
		 * behaviour or predicate function.
		 *
		 *
		 * Rules that apply to this FSM:
		 * 1) Every state has a set of conditions. Each condition has associated state code.
		 *    If condition evaluates to true, it transitions to associated state.
		 * 2) Conditions cannot mutate state, not even BlackboardType.
		 * 3) If all conditions are evaluated to false, default logic runs. Default logic
		 *    is permitted to mutate BlackboardType state.
		 * 4) After default logic executes, then FSM is permitted to transition to another state
		 *    or it keeps the current one.
		 * 4) Each update tick of FSM ends with a transition. So either a condition evaluates to
		 *    true and transitions, or default logic executes and then transitions (keeping the current state
		 *    is also transition). This prevents FSM from freezing the app by constantly looping.
		 */
		template<StateTypeConcept StateType, class ... BlackboardTypes>
		class Fsm final
		{
		private:
			std::map<StateType, State<StateType, BlackboardTypes...>> states;
			StateType currentState = StateType();

			// Logging
			bool loggingEnabled = false;
			std::reference_wrapper<std::ostream> log = std::cout;
			std::map<StateType, std::string> stateToString;

		private:
			[[nodiscard]]
			std::string getStateName(StateType state) const noexcept
			{
				return stateToString.count(state)
					? stateToString.at(state)
					: "UNDEFINED(" + std::to_string(static_cast<int>(state)) + ")";
			}

			void logCurrentState()
			{
				if (!loggingEnabled) return;

				log.get() << std::format(
					"FSM::update(State = {}):\n",
					getStateName(currentState));
			}

			void logTransitionTaken(bool defaultTransition = false, bool loop = false)
			{
				if (!loggingEnabled)
					return;

				log.get() << std::format("  {}, {}{}\n",
					defaultTransition ? "behavior executed" : "condition hit",
					loop ? "looping" : "jumping to ",
					loop ? "" : getStateName(currentState));
			}

		public:
			//Fsm() = default;
			constexpr Fsm(UniversalReference<std::map<StateType, State<StateType, BlackboardTypes...>>> auto&& states)
				: states(std::forward<decltype(states)>(states))
			{}

			void update(BlackboardTypes&... blackboards)
			{
				logCurrentState();

				for (auto&& transition : states[currentState].transitions)
				{
					if (transition.first(blackboards...))
					{
						currentState = transition.second;
						logTransitionTaken();
						return;
					}
				}

				states[currentState].logic(blackboards...);
				const bool looping = currentState == states[currentState].targetState;
				currentState = states[currentState].targetState;
				logTransitionTaken(true, looping);
			}

			constexpr void setState(StateType state) noexcept
			{
				currentState = state;
			}

			void setStateToStringHelper(
				std::map<StateType, std::string>&& _stateToString)
			{
				stateToString = std::move(_stateToString);
			}

			void setStateToStringHelper(
				const std::map<StateType, std::string>& _stateToString)
			{
				stateToString = _stateToString;
			}

			void setLogging(
				bool enabled,
				std::ostream& logger = std::cout)
			{
				loggingEnabled = enabled;
				log = logger;
			}

			[[nodiscard]] constexpr auto getState() const noexcept -> StateType
			{
				return currentState;
			}
		};
	}
}
