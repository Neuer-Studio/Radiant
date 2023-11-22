
project "Radiant"
    kind "StaticLib"

    pchheader "rpch.hpp"
    pchsource "Source/rpch.cpp"
    forceincludes { "rpch.hpp" }

    LibraryDir = {}
    LibraryDir["assimp"] = "../ThirdParty/assimp/bin/Debug/assimp-vc142-mtd.lib"
    LibraryDir["shaderc_Debug"] = "../ThirdParty/VulkanSDK/shaderc/Lib/shadercd.lib"
    LibraryDir["shaderc_shared_Debug"] = "../ThirdParty/VulkanSDK/shaderc/Lib/shaderc_sharedd.lib"
    LibraryDir["shaderc_shared"] = "../ThirdParty/VulkanSDK/shaderc/Lib/shaderc_shared.lib"
    LibraryDir["spirv_cross_core_Debug"] = "../ThirdParty/VulkanSDK/spirv_cross/Lib/spirv-cross-cored.lib"
    LibraryDir["spirv_cross_glsl_Debug"] = "../ThirdParty/VulkanSDK/spirv_cross/Lib/spirv-cross-glsld.lib"
    LibraryDir["vulkan1"] = "../ThirdParty/VulkanSDK/Lib/vulkan-1.lib"
    LibraryDir["shaderc_combined_Debug"] = "../ThirdParty/VulkanSDK/Lib/shaderc_combinedd.lib"
    LibraryDir["shaderc_util_Debug"] = "../ThirdParty/VulkanSDK/Lib/shaderc_utild.lib"

    files { 
        -- Precompiled header
        "Source/rpch.cpp",
        "Source/rpch.hpp",
        
        -- Engine 
        "Source/Engine/**.cpp", 
        "Source/Engine/**.hpp",

        "Include/**.hpp",

        -- stb
        "../ThirdParty/stb/include/stb_image/stb_image.cpp",
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
        "../ThirdParty/glm/",
        "../ThirdParty/assimp/include/",
        "../ThirdParty/stb/include/",
        "../ThirdParty/VulkanSDK/shaderc/Include",
        "../ThirdParty/VulkanSDK/spirv_cross/Include",
        
        "../ThirdParty/",

    }

    links
    {
        "ImGui",
        "%{LibraryDir.assimp}",
        "%{LibraryDir.shaderc_Debug}",
        "%{LibraryDir.spirv_cross_core_Debug}",
        "%{LibraryDir.spirv_cross_glsl_Debug}",
        "%{LibraryDir.shaderc_shared_Debug}",
        "%{LibraryDir.shaderc_combined_Debug}",
        "%{LibraryDir.shaderc_util_Debug}",
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