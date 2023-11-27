#include <Radiant/Rendering/Platform/Vulkan/Local/VulkanDevice.hpp>
#include <Radiant/Rendering/Platform/Vulkan/VulkanContext.hpp>

namespace Radiant
{
	/*
	* Vulkan Physical Device
	*/

	VulkanPhysicalDevice::VulkanPhysicalDevice()
	{
		auto instance = VulkanContext::GetInstance();
		uint32_t GPUCount = 0;
		// Get number of available physical devices
		vkEnumeratePhysicalDevices(instance, &GPUCount, nullptr);
		RADIANT_VERIFY(GPUCount > 0, "");

		std::vector<VkPhysicalDevice> devices(GPUCount);
		if (vkEnumeratePhysicalDevices(instance, &GPUCount, devices.data()) != VK_SUCCESS)
		{
			RADIANT_VERIFY(false);
		}

		// Check for device suitable
		VkPhysicalDevice selectedPhysicalDevice = nullptr;

		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;

		for (const auto& device : devices) {
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
				deviceFeatures.geometryShader)
			{
				selectedPhysicalDevice = device;
				break;
			}

		}

	}

	/*
	* Vulkan Device
	*/

	VulkanDevice::VulkanDevice(const Memory::Shared<VulkanPhysicalDevice>& device)
	{

	}
}
