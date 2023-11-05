#pragma once

#include <DGM/classes/Fsm.hpp>

namespace dgm
{
	namespace fsm
	{
		namespace decorator
		{

			template<class ... BlackboardTypes>
			[[nodiscard]]
			auto And(Condition<BlackboardTypes...> cond1, Condition<BlackboardTypes...> cond2)
			{
				return[cond1, cond2] (const BlackboardTypes&... bb)
					{
						return cond1(bb...) && cond2(bb...);
					};
			}

			template<class ... BlackboardTypes>
			[[nodiscard]]
			auto Or(Condition<BlackboardTypes...> cond1, Condition<BlackboardTypes...> cond2)
			{
				return[cond1, cond2] (const BlackboardTypes&... bb)
					{
						return cond1(bb...) || cond2(bb...);
					};
			}

			template<class ... BlackboardTypes>
			[[nodiscard]]
			auto Not(Condition<BlackboardTypes...> cond1)
			{
				return[cond1] (const BlackboardTypes&... bb)
					{
						return !cond1(bb...);
					};
			}

			template<class ... BlackboardTypes>
			[[nodiscard]]
			auto Merge(Logic<BlackboardTypes...> logic1, Logic<BlackboardTypes...> logic2)
			{
				return [logic1, logic2] (BlackboardTypes&... bb)
					{
						logic1(bb...);
						logic2(bb...);
					};
			}

		}
	}
}