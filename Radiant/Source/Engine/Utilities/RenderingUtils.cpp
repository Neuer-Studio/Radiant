#include <Radiant/Utilities/RenderingUtils.hpp>

#include <stb_image/stb_image.h>

namespace Radiant::Utils
{
	bool Rendering::IsHDR(const std::filesystem::path& file)
	{
		return stbi_is_hdr(file.string().c_str());
	}
}