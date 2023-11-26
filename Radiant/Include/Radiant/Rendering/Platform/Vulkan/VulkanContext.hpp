#pragma once 

#include <Radiant/Rendering/RenderingContext.hpp>

#include <vulkan/vulkan.h>

namespace Radiant
{
	class VulkanContext final : public RenderingContext
	{
	public:
		VulkanContext(GLFWwindow* windowHandle);
		virtual ~VulkanContext() override = default;

		virtual void Init() override;
		virtual void SwapBuffers() const override {}
	private:
		GLFWwindow* m_Window;
		inline static VkInstance s_VulkanInstance;
		VkDebugReportCallbackEXT m_DebugReportCallback = VK_NULL_HANDLE;

	};
}