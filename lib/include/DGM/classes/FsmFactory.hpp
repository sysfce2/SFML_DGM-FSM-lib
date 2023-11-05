#pragma once

#include <string>
#include <map>

#include <DGM/classes/Fsm.hpp>
#include <DGM/classes/FsmLoader.hpp>
#include <DGM/classes/FsmDecorators.hpp>

namespace dgm
{
	namespace fsm
	{
		namespace priv
		{
			std::string getAnnotations(
					const std::vector<std::string>& predicateNames,
					const std::vector<std::string>& behaviorNames
			);

			template<class T>
			static std::vector<std::string> getMapKeys(const std::map<std::string, T>& map)
			{
				std::vector<std::string> keys;
				keys.reserve(map.size());
				for (auto&& [key, _] : map)
					keys.push_back(key);
				return keys;
			}
		}

		template<class ... BlackboardTypes>
		class [[nodiscard]] Factory final
		{
			LoaderInterface& loader;
			std::map<std::string, Condition<BlackboardTypes...>> predicates;
			std::map<std::string, Logic<BlackboardTypes...>> logics;

		public:
			void registerPredicate(
				const std::string& name,
				Condition<BlackboardTypes...> condition)
			{
				predicates[name] = condition;
			}

			void registerLogic(
				const std::string& name,
				Logic<BlackboardTypes...> logic)
			{
				logics[name] = logic;
			}

			[[nodiscard]]
			std::string getAnnotations() const
			{
				return priv::getAnnotations(
					priv::getMapKeys(predicates),
					priv::getMapKeys(logics)
				);
			}

			[[nodiscard]]
			Fsm<unsigned, BlackboardTypes...> loadFromFile(const std::string& path) const
			{
				auto states = loader.loadFromFile(path);

				std::map<unsigned, State<unsigned, BlackboardTypes...>> finalStates;
				auto getCheckedStateId = [&] (unsigned id) -> unsigned
					{
						if (id >= states.size())
						{
							throw std::runtime_error("Referencing state that does not exist");
						}
						return id;
					};

				fsm::Logic<BlackboardTypes...> doNothingStub = [] (BlackboardTypes&...) {};
				for (auto&& [stateId, state] : states)
				{
					auto& fstate = finalStates[stateId];

					for (auto&& transition : state.transitions)
					{
						fstate.transitions.push_back({
							predicates.at(transition.first),
							getCheckedStateId(transition.second) });
					}

					Logic<BlackboardTypes...> finalLogic = state.behaviors.empty()
						? doNothingStub
						: logics.at(state.behaviors[0]);

					for (unsigned i = 1; i < state.behaviors.size(); i++)
					{
						finalLogic = dgm::fsm::decorator::Merge<BlackboardTypes...>(
							finalLogic,
							logics.at(state.behaviors[i]));
					}

					fstate.logic = finalLogic;
					fstate.targetState = getCheckedStateId(state.defaultTransition);
				}

				auto fsm = dgm::fsm::Fsm<unsigned, BlackboardTypes...>(std::move(finalStates));

				// Compute and set log helpers
				std::map<unsigned, std::string> stateToString;
				for (auto&& [index, state] : states)
				{
					stateToString[index] = state.name;
				}
				fsm.setStateToStringHelper(std::move(stateToString));

				return fsm;
			}

			constexpr Factory(LoaderInterface& loader) noexcept
				: loader(loader)
			{}
		};
	}

}