local VulkanSDKPath = os.getenv("VULKAN_SDK") .. "/include"

project "Radiant"
    kind "StaticLib"

    pchheader "rpch.hpp"
    pchsource "Source/rpch.cpp"
    forceincludes { "rpch.hpp" }

    files { 
        -- Precompiled header
        "Source/rpch.cpp",
        "Source/rpch.hpp",
        
        -- Engine 
        "Source/Engine/**.cpp", 
        "Source/Engine/**.hpp",

        "Include/**.hpp"
    }

    includedirs {
        "Source/",
        "Include/",
        "Include/Radiant/",
    }

    includedirs {
        "../ThirdParty/spdlog/include/",
        "../ThirdParty/GLFW/include/",
        "../ThirdParty/Glad/include/",
        "../ThirdParty/ImGui/",
        
        "../ThirdParty/",
        VulkanSDKPath,
    }

    links
    {
        "ImGui"
    }

    filter "configurations:Debug"
        defines { "RADIANT_CONFIG_DEBUG" }
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines { "RADIANT_CONFIG_RELEASE" }

    filter { "system:windows" }
        defines { "RADIANT_PLATFORM_WINDOWS" }

        files {
            "Source/Platform/Windows/**.cpp",
            "Source/Platform/Windows/**.hpp",
        }