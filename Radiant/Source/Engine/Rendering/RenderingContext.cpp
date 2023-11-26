#include <Rendering/RenderingContext.hpp>
#include <Rendering/Platform/OpenGL/OpenGLContext.hpp>
#include <Rendering/Platform/Vulkan/VulkanContext.hpp>

#include <Rendering/RenderingAPI.hpp>

namespace Radiant
{
	
	Memory::Shared<RenderingContext> RenderingContext::Create(GLFWwindow* windowHandle)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None:    return nullptr;
			case RenderingAPIType::Vulkan:  return Memory::Shared<VulkanContext>::Create(windowHandle);
			case RenderingAPIType::OpenGL:  return Memory::Shared<OpenGLContext>::Create(windowHandle);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

}