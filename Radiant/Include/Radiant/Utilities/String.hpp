#pragma once

namespace Radiant::Utils
{
	class StringUtils
	{
	public:
		static std::string ToUpper(const std::string_view& string);
		static std::string FSToString(const std::filesystem::path & string);
	};
}