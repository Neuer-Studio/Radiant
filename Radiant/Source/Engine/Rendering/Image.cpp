#include <Radiant/Rendering/Image.hpp>
#include <Rendering/Platform/OpenGL/OpenGLImage2D.hpp>

#include <Radiant/Rendering/RenderingAPI.hpp>

namespace Radiant
{
	Memory::Ref<Image2D> Image2D::Create(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None: return nullptr;
			case RenderingAPIType::OpenGL: return Memory::Ref<OpenGLImage2D>::Create(format, width, height, buffer);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

}