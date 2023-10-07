#include <Radiant/Rendering/Texture.hpp>
#include <Radiant/Rendering/Platform/OpenGL/OpenGLTexture.hpp>

#include <Radiant/Rendering/RenderingAPI.hpp>

namespace Radiant
{

	Memory::Shared<Texture2D> Texture2D::Create(const std::filesystem::path& path, bool srgb)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None:    return nullptr;
			case RenderingAPIType::OpenGL:  return Memory::Shared<OpenGLTexture2D>::Create(path, srgb);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

}