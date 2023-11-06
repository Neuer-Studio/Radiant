#include <Radiant/Rendering/Image.hpp>
#include <Rendering/Platform/OpenGL/OpenGLImage.hpp>

#include <Radiant/Rendering/RenderingAPI.hpp>

namespace Radiant
{
	Memory::Shared<Image2D> Image2D::Create(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer, uint32_t samples)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None: return nullptr;
			case RenderingAPIType::OpenGL: return Memory::Shared<OpenGLImage2D>::Create(format, width, height, buffer, samples);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

	Memory::Shared<Image2D> Image2D::Create(ImageFormat format, std::size_t width, std::size_t height, const void* buffer /*= nullptr*/, uint32_t samples)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None: return nullptr;
		case RenderingAPIType::OpenGL: return Memory::Shared<OpenGLImage2D>::Create(format, width, height, buffer, samples);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

	Memory::Shared<ImageCube> ImageCube::Create(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None: return nullptr;
		case RenderingAPIType::OpenGL: return Memory::Shared<OpenGLImageCube>::Create(format, width, height, buffer);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

	Memory::Shared<ImageCube> ImageCube::Create(ImageFormat format, std::size_t width, std::size_t height, const std::byte* buffer /*= nullptr*/)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None: return nullptr;
		case RenderingAPIType::OpenGL: return Memory::Shared<OpenGLImageCube>::Create(format, width, height, buffer);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}
}