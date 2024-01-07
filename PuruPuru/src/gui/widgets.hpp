#pragma once
#include "widgets.h"

namespace gui {

	template<>
	inline bool DrawBasicConstrol<float>(float& value, const UISettings<float>& settings)
	{
		const float width = ImGui::GetContentRegionAvail().x;
		ImGui::PushItemWidth(width);
		const bool changed = ImGui::DragFloat("##", &value, settings.Speed, settings.Min, settings.Max);
		ImGui::PopItemWidth();
		return changed;
	}

	template<>
	inline bool DrawBasicConstrol<ImVec2>(ImVec2& value, const UISettings<ImVec2>& settings)
	{
		const auto& theme = ImGui::GetStyle();
		const float width = ImGui::GetContentRegionAvail().x * 0.5f;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

		bool xChanged = DrawRedVectorElement("X", value.x, settings.Reset.x, width, { settings.Min.x, settings.Max.x, settings.Speed.x });
		ImGui::SameLine();
		bool yChanged = DrawGreenVectorElement("Y", value.y, settings.Reset.y, width, { settings.Min.y, settings.Max.y, settings.Speed.y });

		ImGui::PopStyleVar();
		return xChanged || yChanged;
	}

	template<>
	inline bool DrawBasicConstrol<ImVec4>(ImVec4& value, const UISettings<ImVec4>& settings)
	{
		const auto& theme = ImGui::GetStyle();
		const float width = (ImGui::GetContentRegionAvail().x - BUTTON_SIZE - theme.FrameBorderSize * 2.0f) * 0.25f;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

		const bool rChanged = DrawRedVectorElement("R", value.x, settings.Reset.x, width, { settings.Min.x, settings.Max.x, settings.Speed.x });
		ImGui::SameLine();
		const bool gChanged = DrawGreenVectorElement("G", value.y, settings.Reset.y, width, { settings.Min.y, settings.Max.y, settings.Speed.y });
		ImGui::SameLine();
		const bool bChanged = DrawBlueVectorElement("B", value.z, settings.Reset.z, width, { settings.Min.z, settings.Max.z, settings.Speed.z });
		ImGui::SameLine();
		const bool aChanged = DrawAlphaVectorElement(value.w, settings.Reset.w, width, { settings.Min.w, settings.Max.w, settings.Speed.w });
		ImGui::SameLine();
		const bool pChanged = ImGui::ColorEdit4("##Picker", &value.x, ImGuiColorEditFlags_NoInputs);
		ImGui::PopStyleVar();
		return rChanged || gChanged || bChanged || aChanged || pChanged;
	}

	template<typename T>
	inline bool DrawControlRow(const char* name, T& value, const UISettings<T>& settings, const std::string& help)
	{
		ImGui::PushID(name);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::TextUnformatted(name);
		ImGui::TableSetColumnIndex(1);

		const bool changed = DrawBasicConstrol(value, settings);

		ImGui::PopID();
		return changed;
	}

	template<typename Lamda>
	inline void DrawTreeControl(const char* name, Lamda&& lamda)
	{
		constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow;
		if (ImGui::TreeNodeEx(name, treeNodeFlags))
		{
			static constexpr ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV;
			ImGui::BeginTable(name, 2, flags);
			lamda();
			ImGui::EndTable();
			ImGui::TreePop();
			ImGui::Separator();
		}
	}

}