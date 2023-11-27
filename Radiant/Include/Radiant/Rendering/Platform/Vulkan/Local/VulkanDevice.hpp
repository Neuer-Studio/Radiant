#pragma once

#include <vulkan/vulkan.h>

namespace Radiant
{
	/*
	* Vulkan Physical Device 
	*/

	class VulkanPhysicalDevice : public Memory::RefCounted
	{
	public:
		VulkanPhysicalDevice();
	private:
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	};

	/*
	*  Vulkan Deivce
	*/
	class VulkanDevice
	{
	public:
		VulkanDevice(const Memory::Shared<VulkanPhysicalDevice>& device);
	};
}