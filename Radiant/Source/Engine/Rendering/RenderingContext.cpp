#include <Rendering/RenderingContext.hpp>
#include <Rendering/Platform/OpenGL/OpenGLContext.hpp>

#include <Rendering/RenderingAPI.hpp>

namespace Radiant
{
	
	Memory::Ref<RenderingContext> RenderingContext::Create(GLFWwindow* windowHandle)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None:    return nullptr;
			case RenderingAPIType::OpenGL:  return Memory::Ref<OpenGLContext>::Create(windowHandle);
		}
		RADIANT_VERIFY(false, "Unknown RendererAPI");
		return nullptr;
	}

}