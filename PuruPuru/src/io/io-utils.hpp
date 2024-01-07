#pragma once
#include "io-utils.h"

#include <yaml-cpp/yaml.h>

namespace YAML {

	template<>
	struct convert<ImVec2>{
		static Node encode(const ImVec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, ImVec2& vec)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;
			vec.x = node[0].as<float>();
			vec.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<ImVec4> {
		static Node encode(const ImVec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, ImVec4& vec)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;
			vec.x = node[0].as<float>();
			vec.y = node[1].as<float>();
			vec.z = node[2].as<float>();
			vec.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<ImGuiStyle> {
		static Node encode(const ImGuiStyle& rhs)
		{
			Node node;
			// Configs
			node.force_insert("Alpha", rhs.Alpha);
			node.force_insert("DisabledAlpha", rhs.DisabledAlpha);
			node.force_insert("WindowPadding", rhs.WindowPadding);
			node.force_insert("WindowRounding", rhs.WindowRounding);
			node.force_insert("WindowBorderSize", rhs.WindowBorderSize);
			node.force_insert("WindowMinSize", rhs.WindowMinSize);
			node.force_insert("WindowTitleAlign", rhs.WindowTitleAlign);
			//node.force_insert("WindowMenuButtonPosition", rhs.WindowMenuButtonPosition);
			node.force_insert("ChildRounding", rhs.ChildRounding);
			node.force_insert("ChildBorderSize", rhs.ChildBorderSize);
			node.force_insert("PopupRounding", rhs.PopupRounding);
			node.force_insert("PopupBorderSize", rhs.PopupBorderSize);
			node.force_insert("FramePadding", rhs.FramePadding);
			node.force_insert("FrameRounding", rhs.FrameRounding);
			node.force_insert("FrameBorderSize", rhs.FrameBorderSize);
			node.force_insert("ItemSpacing", rhs.ItemSpacing);
			node.force_insert("ItemInnerSpacing", rhs.ItemInnerSpacing);
			node.force_insert("CellPadding", rhs.CellPadding);
			node.force_insert("TouchExtraPadding", rhs.TouchExtraPadding);
			node.force_insert("IndentSpacing", rhs.IndentSpacing);
			node.force_insert("ColumnsMinSpacing", rhs.ColumnsMinSpacing);
			node.force_insert("ScrollbarSize", rhs.ScrollbarSize);
			node.force_insert("ScrollbarRounding", rhs.ScrollbarRounding);
			node.force_insert("GrabMinSize", rhs.GrabMinSize);
			node.force_insert("GrabRounding", rhs.GrabRounding);
			node.force_insert("LayoutAlign", rhs.LayoutAlign);
			node.force_insert("LogSliderDeadzone", rhs.LogSliderDeadzone);
			node.force_insert("TabRounding", rhs.TabRounding);
			node.force_insert("TabBorderSize", rhs.TabBorderSize);
			node.force_insert("TabMinWidthForCloseButton", rhs.TabMinWidthForCloseButton);
			node.force_insert("TabBarBorderSize", rhs.TabBarBorderSize);
			node.force_insert("TableAngledHeadersAngle", rhs.TableAngledHeadersAngle);
			//node.force_insert("ColorButtonPosition", rhs.ColorButtonPosition);
			node.force_insert("ButtonTextAlign", rhs.ButtonTextAlign);
			node.force_insert("SelectableTextAlign", rhs.SelectableTextAlign);
			node.force_insert("SeparatorTextBorderSize", rhs.SeparatorTextBorderSize);
			node.force_insert("SeparatorTextAlign", rhs.SeparatorTextAlign);
			node.force_insert("SeparatorTextPadding", rhs.SeparatorTextPadding);
			node.force_insert("DisplayWindowPadding", rhs.DisplayWindowPadding);
			node.force_insert("DisplaySafeAreaPadding", rhs.DisplaySafeAreaPadding);
			node.force_insert("DockingSeparatorSize", rhs.DockingSeparatorSize);
			node.force_insert("MouseCursorScale", rhs.MouseCursorScale);
			node.force_insert("AntiAliasedLines", rhs.AntiAliasedLines);
			node.force_insert("AntiAliasedLinesUseTex", rhs.AntiAliasedLinesUseTex);
			node.force_insert("AntiAliasedFill", rhs.AntiAliasedFill);
			node.force_insert("CurveTessellationTol", rhs.CurveTessellationTol);
			node.force_insert("CircleTessellationMaxError", rhs.CircleTessellationMaxError);
		
			// Colors
			node.force_insert("ImGuiCol_Text", rhs.Colors[ImGuiCol_Text]);
			node.force_insert("ImGuiCol_TextDisabled", rhs.Colors[ImGuiCol_TextDisabled]);
			node.force_insert("ImGuiCol_WindowBg", rhs.Colors[ImGuiCol_WindowBg]);
			node.force_insert("ImGuiCol_ChildBg", rhs.Colors[ImGuiCol_ChildBg]);
			node.force_insert("ImGuiCol_PopupBg", rhs.Colors[ImGuiCol_PopupBg]);
			node.force_insert("ImGuiCol_Border", rhs.Colors[ImGuiCol_Border]);
			node.force_insert("ImGuiCol_BorderShadow", rhs.Colors[ImGuiCol_BorderShadow]);
			node.force_insert("ImGuiCol_FrameBg", rhs.Colors[ImGuiCol_FrameBg]);
			node.force_insert("ImGuiCol_FrameBgHovered", rhs.Colors[ImGuiCol_FrameBgHovered]);
			node.force_insert("ImGuiCol_FrameBgActive", rhs.Colors[ImGuiCol_FrameBgActive]);
			node.force_insert("ImGuiCol_TitleBg", rhs.Colors[ImGuiCol_TitleBg]);
			node.force_insert("ImGuiCol_TitleBgActive", rhs.Colors[ImGuiCol_TitleBgActive]);
			node.force_insert("ImGuiCol_TitleBgCollapsed", rhs.Colors[ImGuiCol_TitleBgCollapsed]);
			node.force_insert("ImGuiCol_MenuBarBg", rhs.Colors[ImGuiCol_MenuBarBg]);
			node.force_insert("ImGuiCol_ScrollbarBg", rhs.Colors[ImGuiCol_ScrollbarBg]);
			node.force_insert("ImGuiCol_ScrollbarGrab", rhs.Colors[ImGuiCol_ScrollbarGrab]);
			node.force_insert("ImGuiCol_ScrollbarGrabHovered", rhs.Colors[ImGuiCol_ScrollbarGrabHovered]);
			node.force_insert("ImGuiCol_ScrollbarGrabActive", rhs.Colors[ImGuiCol_ScrollbarGrabActive]);
			node.force_insert("ImGuiCol_CheckMark", rhs.Colors[ImGuiCol_CheckMark]);
			node.force_insert("ImGuiCol_SliderGrab", rhs.Colors[ImGuiCol_SliderGrab]);
			node.force_insert("ImGuiCol_SliderGrabActive", rhs.Colors[ImGuiCol_SliderGrabActive]);
			node.force_insert("ImGuiCol_Button", rhs.Colors[ImGuiCol_Button]);
			node.force_insert("ImGuiCol_ButtonHovered", rhs.Colors[ImGuiCol_ButtonHovered]);
			node.force_insert("ImGuiCol_ButtonActive", rhs.Colors[ImGuiCol_ButtonActive]);
			node.force_insert("ImGuiCol_Header", rhs.Colors[ImGuiCol_Header]);
			node.force_insert("ImGuiCol_HeaderHovered", rhs.Colors[ImGuiCol_HeaderHovered]);
			node.force_insert("ImGuiCol_HeaderActive", rhs.Colors[ImGuiCol_HeaderActive]);
			node.force_insert("ImGuiCol_Separator", rhs.Colors[ImGuiCol_Separator]);
			node.force_insert("ImGuiCol_SeparatorHovered", rhs.Colors[ImGuiCol_SeparatorHovered]);
			node.force_insert("ImGuiCol_SeparatorActive", rhs.Colors[ImGuiCol_SeparatorActive]);
			node.force_insert("ImGuiCol_ResizeGrip", rhs.Colors[ImGuiCol_ResizeGrip]);
			node.force_insert("ImGuiCol_ResizeGripHovered", rhs.Colors[ImGuiCol_ResizeGripHovered]);
			node.force_insert("ImGuiCol_ResizeGripActive", rhs.Colors[ImGuiCol_ResizeGripActive]);
			node.force_insert("ImGuiCol_Tab", rhs.Colors[ImGuiCol_Tab]);
			node.force_insert("ImGuiCol_TabHovered", rhs.Colors[ImGuiCol_TabHovered]);
			node.force_insert("ImGuiCol_TabActive", rhs.Colors[ImGuiCol_TabActive]);
			node.force_insert("ImGuiCol_TabUnfocused", rhs.Colors[ImGuiCol_TabUnfocused]);
			node.force_insert("ImGuiCol_TabUnfocusedActive", rhs.Colors[ImGuiCol_TabUnfocusedActive]);
			node.force_insert("ImGuiCol_DockingPreview", rhs.Colors[ImGuiCol_DockingPreview]);
			node.force_insert("ImGuiCol_DockingEmptyBg", rhs.Colors[ImGuiCol_DockingEmptyBg]);
			node.force_insert("ImGuiCol_PlotLines", rhs.Colors[ImGuiCol_PlotLines]);
			node.force_insert("ImGuiCol_PlotLinesHovered", rhs.Colors[ImGuiCol_PlotLinesHovered]);
			node.force_insert("ImGuiCol_PlotHistogram", rhs.Colors[ImGuiCol_PlotHistogram]);
			node.force_insert("ImGuiCol_PlotHistogramHovered", rhs.Colors[ImGuiCol_PlotHistogramHovered]);
			node.force_insert("ImGuiCol_TableHeaderBg", rhs.Colors[ImGuiCol_TableHeaderBg]);
			node.force_insert("ImGuiCol_TableBorderStrong", rhs.Colors[ImGuiCol_TableBorderStrong]);
			node.force_insert("ImGuiCol_TableBorderLight", rhs.Colors[ImGuiCol_TableBorderLight]);
			node.force_insert("ImGuiCol_TableRowBg", rhs.Colors[ImGuiCol_TableRowBg]);
			node.force_insert("ImGuiCol_TableRowBgAlt", rhs.Colors[ImGuiCol_TableRowBgAlt]);
			node.force_insert("ImGuiCol_TextSelectedBg", rhs.Colors[ImGuiCol_TextSelectedBg]);
			node.force_insert("ImGuiCol_DragDropTarget", rhs.Colors[ImGuiCol_DragDropTarget]);
			node.force_insert("ImGuiCol_NavHighlight", rhs.Colors[ImGuiCol_NavHighlight]);
			node.force_insert("ImGuiCol_NavWindowingHighlight", rhs.Colors[ImGuiCol_NavWindowingHighlight]);
			node.force_insert("ImGuiCol_NavWindowingDimBg", rhs.Colors[ImGuiCol_NavWindowingDimBg]);
			node.force_insert("ImGuiCol_ModalWindowDimBg", rhs.Colors[ImGuiCol_ModalWindowDimBg]);
			return node;
		}

		static bool decode(const Node& node, ImGuiStyle& theme)
		{
			theme.Alpha =						node["Alpha"].as<float>();
			theme.DisabledAlpha =				node["DisabledAlpha"].as<float>();
			theme.WindowPadding =				node["WindowPadding"].as<ImVec2>();
			theme.WindowRounding =				node["WindowRounding"].as<float>();
			theme.WindowBorderSize =			node["WindowBorderSize"].as<float>();
			theme.WindowMinSize =				node["WindowMinSize"].as<ImVec2>();
			theme.WindowTitleAlign =			node["WindowTitleAlign"].as<ImVec2>();
			//theme.WindowMenuButtonPosition =	  node["WindowMenuButtonPosition"].as<ImGuiDir>();
			theme.ChildRounding =				node["ChildRounding"].as<float>();
			theme.ChildBorderSize =				node["ChildBorderSize"].as<float>();
			theme.PopupRounding =				node["PopupRounding"].as<float>();
			theme.PopupBorderSize =				node["PopupBorderSize"].as<float>();
			theme.FramePadding =				node["FramePadding"].as<ImVec2>();
			theme.FrameRounding =				node["FrameRounding"].as<float>();
			theme.FrameBorderSize =				node["FrameBorderSize"].as<float>();
			theme.ItemSpacing =					node["ItemSpacing"].as<ImVec2>();
			theme.ItemInnerSpacing =			node["ItemInnerSpacing"].as<ImVec2>();
			theme.CellPadding =					node["CellPadding"].as<ImVec2>();
			theme.TouchExtraPadding =			node["TouchExtraPadding"].as<ImVec2>();
			theme.IndentSpacing =				node["IndentSpacing"].as<float>();
			theme.ColumnsMinSpacing =			node["ColumnsMinSpacing"].as<float>();
			theme.ScrollbarSize =				node["ScrollbarSize"].as<float>();
			theme.ScrollbarRounding =			node["ScrollbarRounding"].as<float>();
			theme.GrabMinSize =					node["GrabMinSize"].as<float>();
			theme.GrabRounding =				node["GrabRounding"].as<float>();
			theme.LayoutAlign =					node["LayoutAlign"].as<float>();
			theme.LogSliderDeadzone =			node["LogSliderDeadzone"].as<float>();
			theme.TabRounding =					node["TabRounding"].as<float>();
			theme.TabBorderSize =				node["TabBorderSize"].as<float>();
			theme.TabMinWidthForCloseButton =	node["TabMinWidthForCloseButton"].as<float>();
			theme.TabBarBorderSize =			node["TabBarBorderSize"].as<float>();
			theme.TableAngledHeadersAngle =		node["TableAngledHeadersAngle"].as<float>();
			//theme.ColorButtonPosition =		  node["ColorButtonPosition"].as<ImGuiDir>();
			theme.ButtonTextAlign =				node["ButtonTextAlign"].as<ImVec2>();
			theme.SelectableTextAlign =			node["SelectableTextAlign"].as<ImVec2>();
			theme.SeparatorTextBorderSize =		node["SeparatorTextBorderSize"].as<float>();
			theme.SeparatorTextAlign =			node["SeparatorTextAlign"].as<ImVec2>();
			theme.SeparatorTextPadding =		node["SeparatorTextPadding"].as<ImVec2>();
			theme.DisplayWindowPadding =		node["DisplayWindowPadding"].as<ImVec2>();
			theme.DisplaySafeAreaPadding =		node["DisplaySafeAreaPadding"].as<ImVec2>();
			theme.DockingSeparatorSize =		node["DockingSeparatorSize"].as<float>();
			theme.MouseCursorScale =			node["MouseCursorScale"].as<float>();
			theme.AntiAliasedLines =			node["AntiAliasedLines"].as<bool>();
			theme.AntiAliasedLinesUseTex =		node["AntiAliasedLinesUseTex"].as<bool>();
			theme.AntiAliasedFill =				node["AntiAliasedFill"].as<bool>();
			theme.CurveTessellationTol =		node["CurveTessellationTol"].as<float>();
			theme.CircleTessellationMaxError =	node["CircleTessellationMaxError"].as<float>();
			
			theme.Colors[ImGuiCol_Text] = node["ImGuiCol_Text"].as<ImVec4>();
			theme.Colors[ImGuiCol_TextDisabled] = node["ImGuiCol_TextDisabled"].as<ImVec4>();
			theme.Colors[ImGuiCol_WindowBg] = node["ImGuiCol_WindowBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_ChildBg] = node["ImGuiCol_ChildBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_PopupBg] = node["ImGuiCol_PopupBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_Border] = node["ImGuiCol_Border"].as<ImVec4>();
			theme.Colors[ImGuiCol_BorderShadow] = node["ImGuiCol_BorderShadow"].as<ImVec4>();
			theme.Colors[ImGuiCol_FrameBg] = node["ImGuiCol_FrameBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_FrameBgHovered] = node["ImGuiCol_FrameBgHovered"].as<ImVec4>();
			theme.Colors[ImGuiCol_FrameBgActive] = node["ImGuiCol_FrameBgActive"].as<ImVec4>();
			theme.Colors[ImGuiCol_TitleBg] = node["ImGuiCol_TitleBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_TitleBgActive] = node["ImGuiCol_TitleBgActive"].as<ImVec4>();
			theme.Colors[ImGuiCol_TitleBgCollapsed] = node["ImGuiCol_TitleBgCollapsed"].as<ImVec4>();
			theme.Colors[ImGuiCol_MenuBarBg] = node["ImGuiCol_MenuBarBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_ScrollbarBg] = node["ImGuiCol_ScrollbarBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_ScrollbarGrab] = node["ImGuiCol_ScrollbarGrab"].as<ImVec4>();
			theme.Colors[ImGuiCol_ScrollbarGrabHovered] = node["ImGuiCol_ScrollbarGrabHovered"].as<ImVec4>();
			theme.Colors[ImGuiCol_ScrollbarGrabActive] = node["ImGuiCol_ScrollbarGrabActive"].as<ImVec4>();
			theme.Colors[ImGuiCol_CheckMark] = node["ImGuiCol_CheckMark"].as<ImVec4>();
			theme.Colors[ImGuiCol_SliderGrab] = node["ImGuiCol_SliderGrab"].as<ImVec4>();
			theme.Colors[ImGuiCol_SliderGrabActive] = node["ImGuiCol_SliderGrabActive"].as<ImVec4>();
			theme.Colors[ImGuiCol_Button] = node["ImGuiCol_Button"].as<ImVec4>();
			theme.Colors[ImGuiCol_ButtonHovered] = node["ImGuiCol_ButtonHovered"].as<ImVec4>();
			theme.Colors[ImGuiCol_ButtonActive] = node["ImGuiCol_ButtonActive"].as<ImVec4>();
			theme.Colors[ImGuiCol_Header] = node["ImGuiCol_Header"].as<ImVec4>();
			theme.Colors[ImGuiCol_HeaderHovered] = node["ImGuiCol_HeaderHovered"].as<ImVec4>();
			theme.Colors[ImGuiCol_HeaderActive] = node["ImGuiCol_HeaderActive"].as<ImVec4>();
			theme.Colors[ImGuiCol_Separator] = node["ImGuiCol_Separator"].as<ImVec4>();
			theme.Colors[ImGuiCol_SeparatorHovered] = node["ImGuiCol_SeparatorHovered"].as<ImVec4>();
			theme.Colors[ImGuiCol_SeparatorActive] = node["ImGuiCol_SeparatorActive"].as<ImVec4>();
			theme.Colors[ImGuiCol_ResizeGrip] = node["ImGuiCol_ResizeGrip"].as<ImVec4>();
			theme.Colors[ImGuiCol_ResizeGripHovered] = node["ImGuiCol_ResizeGripHovered"].as<ImVec4>();
			theme.Colors[ImGuiCol_ResizeGripActive] = node["ImGuiCol_ResizeGripActive"].as<ImVec4>();
			theme.Colors[ImGuiCol_Tab] = node["ImGuiCol_Tab"].as<ImVec4>();
			theme.Colors[ImGuiCol_TabHovered] = node["ImGuiCol_TabHovered"].as<ImVec4>();
			theme.Colors[ImGuiCol_TabActive] = node["ImGuiCol_TabActive"].as<ImVec4>();
			theme.Colors[ImGuiCol_TabUnfocused] = node["ImGuiCol_TabUnfocused"].as<ImVec4>();
			theme.Colors[ImGuiCol_TabUnfocusedActive] = node["ImGuiCol_TabUnfocusedActive"].as<ImVec4>();
			theme.Colors[ImGuiCol_DockingPreview] = node["ImGuiCol_DockingPreview"].as<ImVec4>();
			theme.Colors[ImGuiCol_DockingEmptyBg] = node["ImGuiCol_DockingEmptyBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_PlotLines] = node["ImGuiCol_PlotLines"].as<ImVec4>();
			theme.Colors[ImGuiCol_PlotLinesHovered] = node["ImGuiCol_PlotLinesHovered"].as<ImVec4>();
			theme.Colors[ImGuiCol_PlotHistogram] = node["ImGuiCol_PlotHistogram"].as<ImVec4>();
			theme.Colors[ImGuiCol_PlotHistogramHovered] = node["ImGuiCol_PlotHistogramHovered"].as<ImVec4>();
			theme.Colors[ImGuiCol_TableHeaderBg] = node["ImGuiCol_TableHeaderBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_TableBorderStrong] = node["ImGuiCol_TableBorderStrong"].as<ImVec4>();
			theme.Colors[ImGuiCol_TableBorderLight] = node["ImGuiCol_TableBorderLight"].as<ImVec4>();
			theme.Colors[ImGuiCol_TableRowBg] = node["ImGuiCol_TableRowBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_TableRowBgAlt] = node["ImGuiCol_TableRowBgAlt"].as<ImVec4>();
			theme.Colors[ImGuiCol_TextSelectedBg] = node["ImGuiCol_TextSelectedBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_DragDropTarget] = node["ImGuiCol_DragDropTarget"].as<ImVec4>();
			theme.Colors[ImGuiCol_NavHighlight] = node["ImGuiCol_NavHighlight"].as<ImVec4>();
			theme.Colors[ImGuiCol_NavWindowingHighlight] = node["ImGuiCol_NavWindowingHighlight"].as<ImVec4>();
			theme.Colors[ImGuiCol_NavWindowingDimBg] = node["ImGuiCol_NavWindowingDimBg"].as<ImVec4>();
			theme.Colors[ImGuiCol_ModalWindowDimBg] = node["ImGuiCol_ModalWindowDimBg"].as<ImVec4>();
			
			return true;
		}
	};
}