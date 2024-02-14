#include "widgets.h"

#include <imgui_internal.h>
#include <utils.h>

struct AutocompleteInputText {
	std::string* Text = nullptr;
	const std::vector<std::string>* Suggestions = nullptr;

	AutocompleteInputText(void) = default;
	AutocompleteInputText(std::string* text, const std::vector<std::string>* suggestions)
		: Text(text), Suggestions(suggestions) {}
};

static void DrawRedResetButton(const char* lbl);
static void DrawGreenResetButton(const char* lbl);
static void DrawBlueResetButton(const char* lbl);
static void DrawAlphaResetButton(void);

template<typename Lamda>
static inline bool DrawVectorControl(const char* lbl, float& val, float resetVal, float width, const gui::UISettings<float>& settings, Lamda&& lamda)
{
	const auto& theme = ImGui::GetStyle();

	auto* fgDrawList = ImGui::GetWindowDrawList();
	const auto curr = ImGui::GetCursorScreenPos();
	fgDrawList->AddRectFilled(curr, { curr.x + width, curr.y + BUTTON_SIZE }, ImColor{ theme.Colors[ImGuiCol_FrameBg] }, 4.0f);
	// Reset button
	ImGui::SetNextItemAllowOverlap();
	lamda();
	const bool isResetPressed = ImGui::IsItemClicked(ImGuiMouseButton_Left);
	const bool isResetHovered = ImGui::IsItemHovered(ImGuiMouseButton_Left);
	if (isResetPressed)
		val = resetVal;
	
	// Float control
	ImGui::SameLine();
	ImGui::SetNextItemAllowOverlap();
	ImGui::PushItemWidth(width - BUTTON_SIZE);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
	const bool changed = ImGui::DragFloat(lbl, &val, settings.Speed, settings.Min, settings.Max);
	ImGui::PopStyleVar();
	ImGui::PopItemWidth();
	return isResetPressed || changed;
	//return false;
}

namespace gui {

	bool DrawAutoCompleteControl(std::string& text, const std::vector<std::string>& suggestions)
	{
		static constexpr size_t BUFFER_SIZE = 128;
		static constexpr size_t MIN_AUTO_COMPLETE_LENGTH = 3;

		//	Initialize local array to reduce heap allocation
		//		
		//	This is not a good practice in general as more than one thread should
		//		be able to execute any free function at once. But since this is a
		//		gui draw call I consider that is pretty much imposible to call
		//		this function from more than one thread at once
		static auto sWorking = std::vector<std::string>();
		static int32_t sCurrIndex = -1;

		static char buffer[BUFFER_SIZE];
		memcpy(buffer, text.c_str(), text.size() + 1);

		constexpr ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll
			| ImGuiInputTextFlags_CallbackCompletion
			| ImGuiInputTextFlags_CallbackHistory
			| ImGuiInputTextFlags_CallbackAlways
			| ImGuiInputTextFlags_NoHorizontalScroll;
		
		auto userData = AutocompleteInputText{ &text, &suggestions };
		const bool finished = ImGui::InputText("##AutoComplete", buffer, BUFFER_SIZE, flags, [](ImGuiInputTextCallbackData* data) {
			auto aux = std::string(data->Buf);
			
			if (aux.size() < MIN_AUTO_COMPLETE_LENGTH)
			{
				sCurrIndex = -1;
				return 0;
			}

			auto* userData = (AutocompleteInputText*)data->UserData;
			if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)//Tab
			{
				assert(!sWorking.empty());
				const size_t currIndex = sCurrIndex % sWorking.size();
				const auto& curr = sWorking[currIndex];
				*userData->Text = curr;
				memcpy(buffer, curr.c_str(), curr.size() + 1);
				sWorking.clear();
				sCurrIndex = -2;
				return 1;
			}
			else if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)//Arrows UP/DOWN
			{
				assert(!sWorking.empty());
				const size_t dir = data->EventKey == ImGuiKey_UpArrow ? -1 : 1;
				//sCurrIndex = std::max(sCurrIndex + dir, 0ull) % sWorking.size();
				sCurrIndex = (sCurrIndex + dir + sWorking.size() - 1) % sWorking.size();//Same as the above but more eficient
			}
			else//Always
			{
				sWorking.clear();
				for (const auto& suggestion : *userData->Suggestions)
				{
					const uint64_t dist = distance(suggestion, aux);
					if (dist < MIN_AUTO_COMPLETE_LENGTH)
						sWorking.emplace_back(suggestion);
				}
			}
			return 0;
		}, (void*)&userData);
		
		if (sCurrIndex != -2)
		{
			ImGui::BeginChild("##Suggestions", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 5));
			for (int i = 0; i < sWorking.size(); ++i)
			{
				bool isSelected = (i == sCurrIndex);
				const auto& curr = sWorking[i];
				if (ImGui::Selectable(curr.c_str(), isSelected))
					memcpy(buffer, curr.c_str(), curr.size() + 1);

				if (isSelected)
				{
					// Draw a highlight background for the selected suggestion
					ImU32 color = ImGui::GetColorU32(ImGuiCol_HeaderHovered);
					ImVec2 min = ImGui::GetItemRectMin();
					ImVec2 max = ImGui::GetItemRectMax();
					max.y = min.y + ImGui::GetTextLineHeightWithSpacing();
					ImGui::GetWindowDrawList()->AddRectFilled(min, max, color);
				}
			}
			ImGui::EndChild();
		}
		return false;
	}

	void DrawTableSeparator(int32_t cols)
	{
		ImGui::TableNextRow();
		for (int32_t i = 0; i < cols; i++)
		{
			ImGui::TableSetColumnIndex(i);
			ImGui::Separator();
		}
	}

	bool DrawRedVectorElement(const char* lbl, float& val, float resetVal, float width, const UISettings<float>& settings)
	{		
		return DrawVectorControl("##Red", val, resetVal, width, settings, [lbl]() {
			DrawRedResetButton(lbl);
		});
	}

	bool DrawGreenVectorElement(const char* lbl, float& val, float resetVal, float width, const UISettings<float>& settings)
	{
		return DrawVectorControl("##Green", val, resetVal, width, settings, [lbl]() {
			DrawGreenResetButton(lbl);
		});
	}

	bool DrawBlueVectorElement(const char* lbl, float& val, float resetVal, float width, const UISettings<float>& settings)
	{
		return DrawVectorControl("##Blue", val, resetVal, width, settings, [lbl]() {
			DrawBlueResetButton(lbl);
		});
	}

	bool DrawAlphaVectorElement(float& val, float resetVal, float width, const UISettings<float>& settings)
	{
		return DrawVectorControl("##Alpha", val, resetVal, width, settings, []() {
			DrawAlphaResetButton();
		});
	}

}

static void DrawRedResetButton(const char* lbl)
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::Button(lbl, { BUTTON_SIZE, BUTTON_SIZE });
	ImGui::PopStyleColor(3);
}

static void DrawGreenResetButton(const char* lbl)
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::Button(lbl, { BUTTON_SIZE, BUTTON_SIZE });
	ImGui::PopStyleColor(3);
}

static void DrawBlueResetButton(const char* lbl)
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
	ImGui::Button(lbl, { BUTTON_SIZE, BUTTON_SIZE });
	ImGui::PopStyleColor(3);
}

static void DrawAlphaResetButton(void)
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
	ImGui::Button("A", { BUTTON_SIZE, BUTTON_SIZE });
	ImGui::PopStyleColor(3);
}