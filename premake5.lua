-- Config file for Radiant Engine

workspace "Luminova Studios"
    configurations { "Debug", "Release" }
    architecture "x64"
    startproject "Sanxbox"

    language "C++"
	cppdialect "C++17"
    targetdir "build/Bin/%{cfg.buildcfg}/%{prj.name}"
	objdir "build/Intermediates/%{cfg.buildcfg}/%{prj.name}"

	externalanglebrackets "On"
	externalwarnings "Off"
	warnings "Off"
    staticruntime "On"


include "Radiant/"
include "Sandbox/"

group "ThirdParty"
include "ThirdParty/GLFW/"
include "ThirdParty/Glad/"
include "ThirdParty/ImGui/"
group ""
