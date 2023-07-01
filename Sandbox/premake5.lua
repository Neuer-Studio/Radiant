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
        "../ThirdParty/",
    }

    links{
        "Radiant",
        "GLFW",
    }
    filter "configurations:Debug"
        defines { "RADIANT_CONFIG_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "RADIANT_CONFIG_RELEASE" }
