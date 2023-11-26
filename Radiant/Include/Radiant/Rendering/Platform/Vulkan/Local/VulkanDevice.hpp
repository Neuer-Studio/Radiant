#pragma once

namespace Radiant
{
	/*
	* Vulkan Physical Device 
	*/

	class VulkanPhysicalDevice : public Memory::RefCounted
	{
	public:
		VulkanPhysicalDevice();
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