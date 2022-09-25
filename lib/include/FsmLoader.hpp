#pragma once

#include <vector>
#include <string>
#include <utility>
#include <map>
#include <istream>

namespace dgm
{
	namespace fsm
	{
		namespace loader
		{
			struct State
			{
				std::vector<std::pair<std::string, unsigned>> transitions = {};
				std::vector<std::string> logic = {};
				unsigned defaultTransition = 0;
			};

			using StateMap = std::map<unsigned, State>;
		}

		class LoaderInterface
		{
		public:
			[[nodiscard]]
			virtual loader::StateMap loadFromFile(const std::string&) const = 0;

			virtual ~LoaderInterface() = default;
		};
	}
}