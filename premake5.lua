require "3rdParty/premake-vscode/vscode"
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
IncludeDirs["GLFW"]="%{wks.location}/3rdParty/GLFW/include"

include "3rdParty/imgui-node-editor/ThirdParty/imgui"
include "3rdParty/imgui-node-editor"
include "3rdParty/yaml-cpp"
include "3rdParty/GLFW"

project "PuruPuru"
    kind "ConsoleApp"
    language "C++"
	cppdialect "C++20"
    entrypoint "WinMainCRTStartup"

    targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp",
        "%{IncludeDirs.imnodes}/Examples/Common/Application/Include/*.h",
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

    links { "ImGui", "imgui-node-editor", "yaml-cpp", }

    defines { "IMGUI_DEFINE_MATH_OPERATORS", "NOMINMAX", "_CRT_SECURE_NO_WARNINGS" }

    filter "system:windows"
		systemversion "latest"

        disablewarnings {4311, 4267, 4302}

        files
        {
            "%{IncludeDirs.imnodes}/Examples/Common/Application/Source/DX11/*.h",
            "%{IncludeDirs.imnodes}/Examples/Common/Application/Source/DX11/*.cpp",
        }

        links { "d3d11", "dxgi" }

        defines "PLATFORM_WINDOWS"

    filter "system:linux"
        systemversion "latest"
        pic "On"

        local function getPkgConfigFlags(pkg)
            local cflags = os.outputof("pkg-config --cflags " .. pkg)
            local libs = os.outputof("pkg-config --libs " .. pkg)
            return cflags, libs
        end
      
        -- Get the GTK3 flags for compilation and linking
        local gtk_cflags, gtk_libs = getPkgConfigFlags("gtk+-3.0")

        includedirs
        {
            "%{IncludeDirs.imnodes}/ThirdParty/gl3w/Include",
            "%{IncludeDirs.GLFW}",
            gtk_cflags,
        }

        files
        {
            "%{IncludeDirs.imnodes}/Examples/Common/Application/Source/GLFW/*.h",
            "%{IncludeDirs.imnodes}/Examples/Common/Application/Source/GLFW/*.cpp",    
            "%{IncludeDirs.imnodes}/ThirdParty/gl3w/Source/gl3w.c",
        }

        links
        {
            "GLFW",
            "X11",
            "gtk-3",
            "GL",
            "uuid",
        }

        buildoptions { gtk_cflags }
        libdirs { gtk_libs }
        linkoptions { gtk_libs }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
    filter "configurations:Release"
		runtime "Release"
		optimize "on"