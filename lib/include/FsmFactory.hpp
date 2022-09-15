#pragma once

#include <string>
#include <map>
#include <numeric>
#include <filesystem>

#include "Fsm.hpp"

namespace dgm {

	namespace fsm {

		template<class BlackboardType, EnumConcept StateType>
		class Factory {
			std::map<std::string, Condition<BlackboardType>> predicates;
			std::map<std::string, Logic<BlackboardType>> logics;
			std::map<std::string, StateType> states;

		private:
			template<class T>
			std::string keysToString(const std::map<std::string, T>& map, const std::string& delimiter) const {
				return std::accumulate(
					++(map.begin()),
					map.end(),
					map.begin()->first,
					[&delimiter](const std::string& out, const decltype(*(map.begin())) item) -> std::string {
						return out + delimiter + item.first;
					});
			}

		public:
			void registerPredicate(const std::string& name, Condition<BlackboardType> condition) {
				predicates[name] = condition;
			}

			void registerLogic(const std::string& name, Logic<BlackboardType> logic) {
				logics[name] = logic;
			}

			void registerState(const std::string& name, StateType state) {
				states[name] = state;
			}

			[[nodiscard]]
			std::string getSchema() const {
				const auto seralizedStates = keysToString(states, ",\n");
				const auto serializedPredicates = keysToString(predicates, ",\n");
				const auto seralizedLogics = keysToString(logics, ",\n");

				return std::vformat(
					"{{\n"
					"\"states\": [\n{}\n],\n"
					"\"predicates\": [\n{}\n]\n"
					"\"logics\": [\n{}\n]\n"
					"}}", std::make_format_args(
						seralizedStates,
						serializedPredicates,
						seralizedLogics));
			}

			[[nodiscard]]
			Fsm<BlackboardType, StateType> loadFromJson(const std::filesystem::path& path) const {
			}
		};

	}

}