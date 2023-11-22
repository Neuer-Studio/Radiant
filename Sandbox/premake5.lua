
project "Sandbox"
    kind "ConsoleApp"
    staticruntime "off"

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
        "../ThirdParty/glm/",

        "../ThirdParty/",

    }

    links{
        "Radiant",
        "GLFW",
        "Glad",
        "ImGui",
    }
    filter "configurations:Debug"
        defines { "RADIANT_CONFIG_DEBUG" }
        symbols "On"
        runtime "Debug"
            
    filter "configurations:Release"
    defines { "RADIANT_CONFIG_RELEASE" }
    
    filter { "system:windows" }
    defines { "RADIANT_PLATFORM_WINDOWS" }