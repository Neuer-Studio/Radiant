#include <Radiant/Rendering/Platform/Vulkan/VulkanContext.hpp>

#include <GLFW/glfw3.h>

namespace Radiant
{
#ifdef RA_DEBUG
	static bool s_DebugValidation = true;
#else
	static bool s_DebugValidation = false;
#endif
	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		: m_Window(windowHandle)
	{}

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
	{
		(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
		RA_WARN("VulkanDebugCallback:\n  Object Type: {0}\n  Message: {1}", objectType, pMessage);
		return VK_FALSE;
	}

	void VulkanContext::Init()
	{
		RA_INFO("VulkanContext::Init");
		RADIANT_VERIFY(glfwVulkanSupported(), "GLFW must support Vulkan");

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Radiant"; //TODO(Danya): Fill from Window::GetName()
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Radiant";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		//uint32_t glfwExtensionCount = 0;
		//const char** glfwExtensions;

		//glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

		if (s_DebugValidation)
		{
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		}

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
		createInfo.ppEnabledExtensionNames = instanceExtensions.data();

		if (s_DebugValidation)
		{
			const char* validationLayers = "VK_LAYER_KHRONOS_validation";	

			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			bool validationLayerPresent = false;
			RA_INFO("Vulkan Instance Layers:");
			for (const VkLayerProperties& layer : availableLayers) 
			{
				RA_INFO("  {0}", layer.layerName);
				for (const auto& layerProperties : availableLayers) {
					if (strcmp(layer.layerName, validationLayers) == 0) {
						validationLayerPresent = true;
						break;
					}
				}
			}

			if (validationLayerPresent)
			{
				createInfo.ppEnabledLayerNames = &validationLayers;
				createInfo.enabledLayerCount = 1;
			}
			else
			{
				RA_ERROR("Validation layer VK_LAYER_LUNARG_standard_validation not present, validation is disabled");
			}
		}

		if (vkCreateInstance(&createInfo, nullptr, &s_VulkanInstance) != VK_SUCCESS)
		{
			RADIANT_VERIFY(false);
		}

		if (s_DebugValidation)
		{
			auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(s_VulkanInstance, "vkCreateDebugReportCallbackEXT");
			RADIANT_VERIFY(vkCreateDebugReportCallbackEXT != NULL, "");
			VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
			debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			debug_report_ci.pfnCallback = VulkanDebugReportCallback;
			debug_report_ci.pUserData = NULL;
			if (vkCreateDebugReportCallbackEXT(s_VulkanInstance, &debug_report_ci, nullptr, &m_DebugReportCallback) != VK_SUCCESS)
			{
				RADIANT_VERIFY(false);
			}
		}
	}

	VulkanContext::~VulkanContext()
	{
		vkDestroyInstance(s_VulkanInstance, nullptr);
		s_VulkanInstance = nullptr;
	}

}