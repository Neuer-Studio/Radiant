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
		struct QueueFamilyIndices
		{
			int32_t GraphicsFamily = -1;
			int32_t ComputeFamily = -1;
			int32_t TransferFamily = -1;
		};
		VulkanPhysicalDevice();
		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		bool IsExtensionSupported(const std::string& extensionName) const;
		VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_PhysicalDevice; }
	private:
		VkFormat FindDepthFormat() const;
		QueueFamilyIndices GetQueueFamilyIndices(int queueFlags);
	private:
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::unordered_set<std::string> m_SupportedExtensions;
		std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;

		VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;

		QueueFamilyIndices m_QueueFamilyIndices;
	public:
		static Memory::Shared<VulkanPhysicalDevice> Create();

		friend class VulkanDevice;
	};

	/*
	*  Vulkan Deivce
	*/
	class VulkanDevice : public Memory::RefCounted
	{
	public:
		VulkanDevice(const Memory::Shared<VulkanPhysicalDevice>& physicalDevice);
		~VulkanDevice();

		void Destroy();

		const Memory::Shared<VulkanPhysicalDevice>& GetPhysicalDevice() const { return m_PhysicalDevice; }
		VkDevice GetVulkanDevice() const { return m_LogicalDevice; }
	private:
		Memory::Shared<VulkanPhysicalDevice> m_PhysicalDevice;
		VkDevice m_LogicalDevice = nullptr;
		VkCommandPool m_CommandPool, m_ComputeCommandPool;
		VkPhysicalDeviceFeatures m_EnabledFeatures;

		bool m_EnableDebugMarkers;

		VkQueue m_Queue;
		VkQueue m_ComputeQueue;
	public:
		static Memory::Shared<VulkanDevice> Create(const Memory::Shared<VulkanPhysicalDevice>& physicalDevice);
	};
}