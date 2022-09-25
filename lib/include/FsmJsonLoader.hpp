#include <FsmLoader.hpp>

namespace dgm
{

	namespace fsm
	{
		class JsonLoader final : public LoaderInterface
		{
		public:
			[[nodiscard]]
			virtual loader::StateMap loadFromFile(const std::string&) const override;

			[[nodiscard]]
			loader::StateMap
				loadFromStream(std::istream&) const;
		};
	}

}