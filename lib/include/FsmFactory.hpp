#pragma once

#include <string>
#include <map>
#include <numeric>

#include <Fsm.hpp>
#include <FsmLoader.hpp>
#include <FsmBuilder.hpp>

namespace dgm
{

	namespace fsm
	{

		template<class BlackboardType>
		class Factory final
		{
			LoaderInterface& loader;
			std::map<std::string, Condition<BlackboardType>> predicates;
			std::map<std::string, Logic<BlackboardType>> logics;

		private:
			template<class T>
			std::string keysToString(const std::map<std::string, T>& map, const std::string& delimiter) const
			{
				return std::accumulate(
					++(map.begin()),
					map.end(),
					map.begin()->first,
					[&delimiter] (const std::string& out, const decltype(*(map.begin())) item) -> std::string
 {
	 return out + delimiter + item.first;
					});
			}

		public:
			void registerPredicate(const std::string& name, Condition<BlackboardType> condition)
			{
				predicates[name] = condition;
			}

			void registerLogic(const std::string& name, Logic<BlackboardType> logic)
			{
				logics[name] = logic;
			}

			[[nodiscard]]
			std::string getSchema() const
			{
				const auto serializedPredicates = keysToString(predicates, ",\n");
				const auto seralizedLogics = keysToString(logics, ",\n");

				return std::vformat(
					"{{\n"
					"\"predicates\": [\n{}\n]\n"
					"\"logics\": [\n{}\n]\n"
					"}}", std::make_format_args(
						serializedPredicates,
						seralizedLogics));
			}

			[[nodiscard]]
			Fsm<BlackboardType, unsigned> loadFromFile(const std::string& path) const
			{
				auto states = loader.loadFromFile(path);
				auto builder = dgm::fsm::Builder<BlackboardType, unsigned>();

				std::map<unsigned, State<BlackboardType, unsigned>> finalStates;
				auto getCheckedStateId = [&] (unsigned id) -> unsigned
				{
					if (id >= states.size())
					{
						throw std::runtime_error("Referencing state that does not exist");
					}
					return id;
				};

				for (auto&& [stateId, state] : states)
				{
					auto& fstate = finalStates[stateId];

					for (auto&& transition : state.transitions)
					{
						fstate.transitions.push_back({
							predicates.at(transition.first),
							getCheckedStateId(transition.second) });
					}

					// TODO: merging logics
					fstate.logic = logics.at(state.logic[0]);
					fstate.targetState = getCheckedStateId(state.defaultTransition);
				}

				return dgm::fsm::Fsm(std::move(finalStates));
			}

			Factory(LoaderInterface& loader)
				: loader(loader)
			{}
		};

	}

}