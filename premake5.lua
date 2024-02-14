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
IncludeDirs["imgui"]="%{wks.location}/3rdParty/imgui"
IncludeDirs["stbi"]="%{wks.location}/3rdParty/imgui-node-editor/external/stb_image"
IncludeDirs["imnodes"]="%{wks.location}/3rdParty/imgui-node-editor"
IncludeDirs["IconHeader"]="%{wks.location}/3rdParty/IconFontCppHeaders"
--IncludeDirs["gtest"]="%{wks.location}/3rdParty/googletest"
--IncludeDirs["entt"]="%{wks.location}/3rdParty/entt/single_include"
IncludeDirs["yaml"]="%{wks.location}/3rdParty/yaml-cpp/include"
IncludeDirs["glfw"]="%{wks.location}/3rdParty/glfw/include"
IncludeDirs["glad"]="%{wks.location}/3rdParty/glad/include"

include "3rdParty/imgui"
include "3rdParty/imgui-node-editor"
include "3rdParty/yaml-cpp"
include "3rdParty/glad"
include "3rdParty/glfw"

project "PuruPuru"
    location "PuruPuru"
    kind "ConsoleApp"
    language "C++"
	cppdialect "C++17"
    entrypoint "WinMainCRTStartup"

    targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    disablewarnings {4311, 4267, 4302}

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
		
        "%{IncludeDirs.imnodes}/examples/application/source/imgui_impl_opengl3.h",
		"%{IncludeDirs.imnodes}/examples/application/source/imgui_impl_glfw.h",
		"%{IncludeDirs.imnodes}/examples/application/source/imgui_impl_opengl3.cpp",
		"%{IncludeDirs.imnodes}/examples/application/source/imgui_impl_glfw.cpp",

        "%{IncludeDirs.imnodes}/examples/application/source/platform_glfw.cpp",
        "%{IncludeDirs.imnodes}/examples/application/source/renderer_ogl3.cpp",

        --"%{IncludeDirs.imnodes}/examples/application/source/application.cpp",
        --"%{IncludeDirs.imnodes}/examples/application/source/entry_point.cpp",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDirs.stbi}",
        "%{IncludeDirs.glad}",
        "%{IncludeDirs.glfw}",
        "%{IncludeDirs.imgui}",
        "%{IncludeDirs.imnodes}",
        "%{IncludeDirs.imnodes}/examples/application/include/",
        "%{IncludeDirs.imnodes}/examples/application/source/",
        "%{IncludeDirs.imnodes}/examples/blueprints-example/",
        "%{IncludeDirs.yaml}",
        "%{IncludeDirs.IconHeader}",
    }

    links { "imgui-node-editor", "glfw", "opengl32", "yaml-cpp", "Glad" }

    defines { "_CRT_SECURE_NO_WARNINGS", "IMGUI_IMPL_OPENGL_LOADER_GLAD" }

    filter "system:windows"
		systemversion "latest"
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
    filter "configurations:Release"
		runtime "Release"
		optimize "on"

--project "tests"
--    location "tests"
--    kind "ConsoleApp"
--    language "C++"
--    cppdialect "C++17"
--
--    targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
--    objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
--
--    files
--    {
--        "%{prj.name}/src/**.cpp",
--        "PuruPuru/src/**.cpp",
--
--        "%{IncludeDirs.gtest}/googletest/src/**.cc",
--    }
--
--    removefiles {
--        "PuruPuru/src/app.cpp",
--        "%{IncludeDirs.gtest}/googletest/src/gtest-all.cc",
--    }
--
--    includedirs
--    {
--        "PuruPuru/src",
--        "%{IncludeDirs.imgui}",
--        "%{IncludeDirs.imnodes}",
--        "%{IncludeDirs.imnodes}/examples/application/include/",
--
--
--        "%{IncludeDirs.gtest}/googletest",
--        "%{IncludeDirs.gtest}/googletest/include",
--    }
--
--    links { "ImGui", "imgui-node-editor" }
--
--    defines { "_CRT_SECURE_NO_WARNINGS" }
--
--    filter "system:windows"
--        systemversion "latest"
--    filter "configurations:Debug"
--        runtime "Debug"
--        symbols "on"
--    filter "configurations:Release"
--        runtime "Release"
--        optimize "on"