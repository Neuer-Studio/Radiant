#pragma once 

#include <Radiant/Rendering/RenderingContext.hpp>

#include <vulkan/vulkan.h>
#include <Rendering/Platform/Vulkan/Local/VulkanDevice.hpp>

namespace Radiant
{
	class VulkanContext final : public RenderingContext
	{
	public:
		VulkanContext(GLFWwindow* windowHandle);
		virtual ~VulkanContext() override;

		static VkInstance GetInstance() { return s_VulkanInstance; }

		virtual void Init() override;
		virtual void SwapBuffers() const override {}
	private:
		GLFWwindow* m_Window;
		inline static VkInstance s_VulkanInstance;
		VkDebugReportCallbackEXT m_DebugReportCallback = VK_NULL_HANDLE;

		// Devices
		Memory::Shared<VulkanPhysicalDevice> m_PhysicalDevice;
		Memory::Shared<VulkanDevice> m_Device;
	};
}