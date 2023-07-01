-- Config file for Radiant Engine

workspace "Luminova Studios"
    configurations { "Debug", "Release" }
    architecture "x64"
    startproject "Sanxbox"

    language "C++"
	cppdialect "C++17"
    targetdir "build/Bin/%{cfg.buildcfg}"
	objdir "build/Intermediates/%{cfg.buildcfg}"

	externalanglebrackets "On"
	externalwarnings "Off"
	warnings "Off"


include "Radiant/"
include "Sandbox/"
