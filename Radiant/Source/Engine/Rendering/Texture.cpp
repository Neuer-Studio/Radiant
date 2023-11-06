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

	Memory::Shared<Texture2D> Texture2D::Create(ImageFormat format, uint32_t width, uint32_t height, const void* data)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None:    return nullptr;
		case RenderingAPIType::OpenGL:  return Memory::Shared<OpenGLTexture2D>::Create(format, width, height, data);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

	Memory::Shared<TextureCube> TextureCube::Create(const std::filesystem::path& path)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None:    return nullptr;
		case RenderingAPIType::OpenGL:  return Memory::Shared<OpenGLTextureCube>::Create(path);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

	Memory::Shared<TextureCube> TextureCube::Create(ImageFormat format, uint32_t width, uint32_t height)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None:    return nullptr;
		case RenderingAPIType::OpenGL:  return Memory::Shared<OpenGLTextureCube>::Create(format, width, height);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}
}