#pragma once

#include <string>
#include <vector>
#include <imgui.h>

static constexpr inline float BUTTON_SIZE = 26.0f;

namespace gui {

	template<typename T>
	struct UISettings {
		T Min = 0;
		T Max = 0;
		T Speed = 1;
	};

	template<>
	struct UISettings<ImVec2> {
		ImVec2 Min = { 0.0f, 0.0f };
		ImVec2 Max = { 0.0f, 0.0f };
		ImVec2 Speed = { 1.0f, 1.0f };

		ImVec2 Reset = { 0.0f, 0.0f };
	};

	template<>
	struct UISettings<ImVec4> {
		ImVec4 Min = { 0.0f, 0.0f, 0.0f, 0.0f };
		ImVec4 Max = { 1.0f, 1.0f, 1.0f, 1.0f };
		ImVec4 Speed = { 0.05f, 0.05f, 0.05f, 0.05f };

		ImVec4 Reset = { 0.0f, 0.0f, 0.0f, 1.0f };
	};

	template<>
	struct UISettings<std::string> {
		size_t Length = 0;
	};

	bool DrawAutoCompleteControl(std::string& text, const std::vector<std::string>& suggestions);

	void DrawTableSeparator(int32_t cols);

	bool DrawRedVectorElement(const char* lbl, float& val, float resetVal, float width, const UISettings<float>& settings);
	bool DrawGreenVectorElement(const char* lbl, float& val, float resetVal, float width, const UISettings<float>& settings);
	bool DrawBlueVectorElement(const char* lbl, float& val, float resetVal, float width, const UISettings<float>& settings);
	bool DrawAlphaVectorElement(float& val, float resetVal, float width, const UISettings<float>& settings);

	template<typename T>
	bool DrawBasicConstrol(T& value, const UISettings<T>& settings) { static_assert(false, "Not implemented yet!"); }

	template<typename T>
	bool DrawControlRow(const char* name, T& value, const UISettings<T>& settings, const std::string& help = "");

	template<typename Lamda>
	void DrawTreeControl(const char* name, Lamda&& lamda);
}

#include "widgets.hpp"