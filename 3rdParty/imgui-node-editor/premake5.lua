project "imgui-node-editor"
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"NodeEditor/Source/*.h",
		"NodeEditor/Source/*.cpp",
		"NodeEditor/Include/*.h",
	}

    includedirs
	{
		"%{IncludeDirs.imgui}",
		"%{IncludeDirs.imnodes}/NodeEditor/Include"
	}

    --defines { "IMGUI_DEFINE_MATH_OPERATORS" }

    links { "ImGui" }

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "On"
		systemversion "latest"
	filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
    filter "configurations:Release"
		runtime "Release"
		optimize "on"