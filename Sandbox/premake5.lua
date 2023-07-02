local VulkanSDKPath = os.getenv("VULKAN_SDK") .. "/include"

project "Sandbox"
    kind "ConsoleApp"

    files { 
        -- Engine 
        "Source/**.cpp", 
        "Source/**.hpp",
    }

    includedirs {
        "../Radiant/Include/",

        "../ThirdParty/spdlog/include/",
        "../ThirdParty/GLFW/include/",
        "../ThirdParty/Glad/include/",

        "../ThirdParty/",

        VulkanSDKPath,
    }

    links{
        "Radiant",
        "GLFW",
        "Glad",
    }
    filter "configurations:Debug"
        defines { "RADIANT_CONFIG_DEBUG" }
        symbols "On"
            
    filter "configurations:Release"
    defines { "RADIANT_CONFIG_RELEASE" }
    
    filter { "system:windows" }
    defines { "RADIANT_PLATFORM_WINDOWS" }