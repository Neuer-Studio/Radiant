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
        "../ThirdParty/",
    }

    filter "configurations:Debug"
        defines { "RADIANT_CONFIG_DEBUG" }

    filter "configurations:Release"
        defines { "RADIANT_CONFIG_RELEASE" }
