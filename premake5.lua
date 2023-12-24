workspace "PuruPuru"

	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
	}
    
    startproject  "PuruPuru"

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

IncludeDirs={}
IncludeDirs["imgui"]="%{wks.location}/3rdParty/imgui-node-editor/ThirdParty/imgui"
IncludeDirs["imnodes"]="%{wks.location}/3rdParty/imgui-node-editor"
IncludeDirs["entt"]="%{wks.location}/3rdParty/entt/single_include"
IncludeDirs["yaml"]="%{wks.location}/3rdParty/yaml-cpp/include"

include "3rdParty/imgui-node-editor/ThirdParty/imgui"
include "3rdParty/imgui-node-editor"
include "3rdParty/yaml-cpp"

project "PuruPuru"
    kind "ConsoleApp"
    language "C++"
	cppdialect "C++17"
    entrypoint "WinMainCRTStartup"

    targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    disablewarnings {4311, 4267, 4302}

    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp",
        "%{IncludeDirs.imnodes}/Examples/Common/Application/Include/*.h",
        "%{IncludeDirs.imnodes}/Examples/Common/Application/Source/DX11/*.h",
        "%{IncludeDirs.imnodes}/Examples/Common/Application/Source/DX11/*.cpp",
        "%{IncludeDirs.imnodes}/Examples/Common/BlueprintUtilities/Source/*.h",
        "%{IncludeDirs.imnodes}/Examples/Common/BlueprintUtilities/Source/*.cpp",
        "%{IncludeDirs.imnodes}/Examples/Common/BlueprintUtilities/Source/ax/*.h",
        "%{IncludeDirs.imnodes}/Examples/Common/BlueprintUtilities/Source/ax/*.cpp",
    }

    includedirs
    {
        "includes",
        "%{IncludeDirs.yaml}",
        "%{IncludeDirs.entt}",
        "%{IncludeDirs.imgui}",
        "%{IncludeDirs.imgui}/backends",
        "%{IncludeDirs.imnodes}/NodeEditor/Include",
        "%{IncludeDirs.imnodes}/ThirdParty/ScopeGuard",
        "%{IncludeDirs.imnodes}/ThirdParty/stb_image",
        "%{IncludeDirs.imnodes}/Examples/Blueprints",
        "%{IncludeDirs.imnodes}/Examples/Application/Include",
        "%{IncludeDirs.imnodes}/Examples/Common/Application/Include",
        "%{IncludeDirs.imnodes}/Examples/Common/BlueprintUtilities/Include",
        "%{IncludeDirs.imnodes}/Examples/Common/BlueprintUtilities/Source",
    }

    links { "ImGui", "imgui-node-editor", "yaml-cpp", "d3d11", "dxgi" }

    defines { "IMGUI_DEFINE_MATH_OPERATORS", "NOMINMAX", "_CRT_SECURE_NO_WARNINGS" }

    filter "system:windows"
		systemversion "latest"
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
    filter "configurations:Release"
		runtime "Release"
		optimize "on"