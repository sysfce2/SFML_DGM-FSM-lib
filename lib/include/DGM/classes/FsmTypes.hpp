#pragma once

#include <concepts>
#include <functional>
#include <utility>
#include <vector>
#include <type_traits>

namespace dgm
{
	namespace fsm
	{
		template<typename T>
		concept StateTypeConcept = std::is_scoped_enum_v<T> || (std::is_arithmetic_v<T> && std::is_unsigned_v<T>);

		template<class T, class Ref>
		concept UniversalReference = std::is_same_v<Ref, T> || std::constructible_from<Ref, T>;

		template<class ... BlackboardTypes>
		using Condition = std::function<bool(const BlackboardTypes&...)>;

		template<class ... BlackboardTypes>
		using Logic = std::function<void(BlackboardTypes&...)>;

		template<StateTypeConcept StateType, class ... BlackboardTypes>
		using Transition = std::pair<Condition<BlackboardTypes...>, StateType>;

		template<StateTypeConcept StateType, class ... BlackboardTypes>
		struct State
		{
			std::vector<Transition<StateType, BlackboardTypes...>> transitions;
			Logic<BlackboardTypes...> logic;
			StateType targetState = StateType();
		};
	}
}