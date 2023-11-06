#include <Radiant/Utilities/String.hpp>

namespace Radiant::Utils
{
	std::string StringUtils::ToUpper(const std::string_view& string)
	{
		std::string result;
		for (const auto& character : string)
		{
			result += std::toupper(character);
		}

		return result;
	}
}