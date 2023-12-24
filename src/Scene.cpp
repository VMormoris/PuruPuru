#include "Scene.h"
#include "SceneSerializer.h"
#include "ExportSerializer.h"
#include <FileDialog.h>

#include <filesystem>

#include <imgui_internal.h>

std::array<bool, 7> Scene::sWindows = { true, true, true, true, true, true, true };

using namespace ax;

void ShowStyleEditor(bool* show = nullptr);
void TextWithBackgroundColor(const char* text, const ImVec4& bgColor);

void Scene::RenderFrame(void) noexcept
{
    mAllData[mWorkingDataIndex].Self.UpdateTouch();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) SaveAs();
            if (ImGui::MenuItem("Save", "Ctrl+S")) Save();
            if (ImGui::MenuItem("Open...", "Ctrl+O")) Open();
            ImGui::Separator();
            if (ImGui::MenuItem("Export..."))
            {
                std::filesystem::path filepath = CreateFileDialog(FileDialogType::Save);
                if (!filepath.empty())
                    ExportSerializer{ this }.Serialize(filepath.string());
            }
            if (ImGui::MenuItem("Export Lines..."))
            {
                std::filesystem::path filepath = CreateFileDialog(FileDialogType::Save);
                if (!filepath.empty())
                    ExportSerializer{ this }.SerializeLines(filepath.string());
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Characters", nullptr, &sWindows[CHARACTERS_INDEX]);
            ImGui::MenuItem("Nodes", nullptr, &sWindows[NODE_EDITOR_INDEX], false);
            ImGui::MenuItem("Acts", nullptr, &sWindows[ACT_INDEX]);
            ImGui::MenuItem("Expressions", nullptr, &sWindows[EXPRESSION_INDEX]);
            ImGui::MenuItem("Dialogues", nullptr, &sWindows[DIALOGUE_INDEX]);
            ImGui::MenuItem("Variables", nullptr, &sWindows[VARIABLE_INDEX]);
            ImGui::MenuItem("Quests", nullptr, &sWindows[QUEST_INDEX]);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    auto& io = ImGui::GetIO();
    const bool ctrl = io.KeyCtrl;
    const bool shift = io.KeyShift;

    if (ctrl && shift && ImGui::IsKeyDown(ImGuiKey_S))
        SaveAs();
    if (ctrl && ImGui::IsKeyDown(ImGuiKey_S))
        Save();
    if (ctrl && ImGui::IsKeyDown(ImGuiKey_O))
        Open();

    ShowPanels();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Node Editor", nullptr);
    ed::SetCurrentEditor(mAllData[mWorkingDataIndex].Editor);
    ed::Begin("Node Editor");
    {
        auto cursorTopLeft = ImGui::GetCursorScreenPos();
        mAllData[mWorkingDataIndex].Self.RenderNodes();
        mAllData[mWorkingDataIndex].Self.RenderLinks();
        mAllData[mWorkingDataIndex].Self.RenderCreatePanel();
        ImGui::SetCursorScreenPos(cursorTopLeft);
    }

    mAllData[mWorkingDataIndex].Self.HandleInput();

    ed::End();
    ImGui::End();
    ImGui::PopStyleVar();
}

void Scene::ShowPanels()
{
    static Flavor mainCharacterFlavor = Flavor::Neutral;
    static bool showStyleEditor = false;

    auto& io = ImGui::GetIO();
    if (sWindows[CHARACTERS_INDEX])
    {
        if (ImGui::Begin("Characters", &sWindows[CHARACTERS_INDEX]))
        {
            if (mDebuging)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
            }

            if (ImGui::Button("Zoom to Content"))
                ed::NavigateToContent();

            ImGui::SameLine();
            //if (ImGui::Button("Show Flow"))
            //    mAllData[mWorkingDataIndex].Self.RenderFlow();
            //
            //if (ImGui::Button("Edit Style"))
            //    showStyleEditor = true;

            //if (showStyleEditor)
            //    ShowStyleEditor(&showStyleEditor);

            if (ImGui::Button("Add"))
            {
                ed::Config config;
                auto* editor = ed::CreateEditor(&config);
                ed::SetCurrentEditor(editor);
                mAllData.emplace_back(CharacterData{ editor });
                ed::SetCurrentEditor(mAllData[mWorkingDataIndex].Editor);
            }

            if (mDebuging)
            {
                ImGui::PopStyleVar();
                ImGui::PopItemFlag();
            }

            ImGui::SameLine();
            if (ImGui::Button(mDebuging ? "Stop" : "Debug"))
            {
                mDebuging = !mDebuging;
                if (!mDebuging)
                    mStateMachine.Clear();
                else
                {
                    for (const auto& data : mAllData)
                        data.Self.SetupVariables(mStateMachine);
                }
            }

            size_t delIndex = INVALID_ID;
            for (size_t i = 0; i < mAllData.size(); i++)
            {
                auto& data = mAllData[i];
                const std::string btnLabel = "X##" + std::to_string(i);
                ImGui::PushStyleColor(ImGuiCol_Button, { 0.9f, 0.125f, 0.213f, 1.0f });
                if (mAllData.size() != 1)
                {
                    if (ImGui::Button(btnLabel.c_str(), { 24, 24 }))
                        delIndex = i;
                    ImGui::SameLine();
                }
                ImGui::PopStyleColor();

                if (i == mEditingIndex)
                    ImGui::InputText("##CharacterName", data.Name, 64);
                else
                    TextWithBackgroundColor(data.Name, i == mWorkingDataIndex ? ImVec4{0.33f, 0.33f, 0.33f, 1.0f} : ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    mEditingIndex = i;
                else if (ImGui::IsItemClicked() && mEditingIndex != i)
                {
                    mWorkingDataIndex = i;
                    mEditingIndex = INVALID_ID;
                }

                if (mDebuging)
                {
                    ImGui::SameLine();
                    ImGui::PushID(i);
                    if (ImGui::Button("Speak"))
                    {
                        mWorkingDataIndex = i;
                        mDebugingID = mAllData[mWorkingDataIndex].Self.GetEntryNodeID();
                        mDone = true;
                        mSpeaking = true;
                    }
                    ImGui::PopID();
                }
            }

            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && ImGui::IsAnyItemHovered())
                mEditingIndex = INVALID_ID;

            if (delIndex != INVALID_ID)
            {
                mAllData.erase(mAllData.begin() + delIndex);
                if (mEditingIndex == delIndex)
                    mEditingIndex = INVALID_ID;

                if (mWorkingDataIndex == delIndex)
                    mWorkingDataIndex = 0;
            }
        }
        ImGui::End();
    }

    if (mDebuging && sWindows[VARIABLE_INDEX])
    {
        if (ImGui::Begin("Variables", &sWindows[VARIABLE_INDEX]))
        {
            constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow;//| ImGuiTreeNodeFlags_SpanAvailWidth
            const bool booleans = ImGui::TreeNodeEx("Booleans", treeNodeFlags);
            if (booleans)
            {
                for (auto&& [var, val] : mStateMachine.Booleans())
                {
                    ImGui::Text(var.c_str()); ImGui::SameLine();
                    const auto lbl = std::string("##") + var;
                    ImGui::Checkbox(lbl.c_str(), &val);
                }
                ImGui::TreePop();
            }
            
            const bool integers = ImGui::TreeNodeEx("Integers", treeNodeFlags);
            if (integers)
            {
                for (auto&& [var, val] : mStateMachine.Integers())
                {
                    ImGui::Text(var.c_str()); ImGui::SameLine();
                    const auto lbl = std::string("##") + var;
                    ImGui::DragInt(lbl.c_str(), &val);
                }
                ImGui::TreePop();
            }

            const bool forks = ImGui::TreeNodeEx("Forks", treeNodeFlags);
            if (forks)
            {
                for (auto&& [var, val] : mStateMachine.Forks())
                {
                    ImGui::Text(var.str().c_str()); ImGui::SameLine();
                    const auto lbl = std::string("##") + var.str();
                    ImGui::Checkbox(lbl.c_str(), &val);
                }
                ImGui::TreePop();
            }

            const bool flavors = ImGui::TreeNodeEx("Flavors", treeNodeFlags);
            if (flavors)
            {
                static constexpr char* Flavors[] = { "Bitter", "Salty", "Sour", "Sweet", "Neutral" };
                
                ImGui::Text("Main Character"); ImGui::SameLine();
                int32_t index = static_cast<int32_t>(mainCharacterFlavor);
                if (ImGui::Combo("##Main Character", &index, Flavors, IM_ARRAYSIZE(Flavors)))
                    mainCharacterFlavor = static_cast<Flavor>(index);
                
                for (auto& data : mAllData)
                {
                    ImGui::Text(data.Name); ImGui::SameLine();
                    const auto& lbl = std::string("##") + data.Name;
                    int32_t index = static_cast<int32_t>(data.CharacterFlavor);
                    if (ImGui::Combo(lbl.c_str(), &index, Flavors, IM_ARRAYSIZE(Flavors)))
                        data.CharacterFlavor = static_cast<Flavor>(index);
                }
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    if (mSpeaking && sWindows[DIALOGUE_INDEX])
    {   
        if (ImGui::Begin("Dialogues", &sWindows[DIALOGUE_INDEX]))
        {
            auto& character = mAllData[mWorkingDataIndex].Self;
            auto npcFlavor = mAllData[mWorkingDataIndex].CharacterFlavor;
            auto curr = character.FindEntity(Node{mDebugingID}.ID);
            auto pair = std::make_pair(character.FindNodes(AnyNode{}, curr), character.FindNodeType(curr));
            if (mDone)
            {
                do
                {
                    pair = mAllData[mWorkingDataIndex].Self.FindNextNode((int32_t)pair.first->ID.AsPointer(), mStateMachine, std::make_pair(mainCharacterFlavor, npcFlavor), mChoice);
                }
                while (pair.second != NodeType::Act && pair.second != NodeType::Dialogue && pair.second != NodeType::None);
                mDone = pair.second == NodeType::None;
                if (!mDone)
                    mDebugingID = (int32_t)pair.first->ID.AsPointer();
                if (pair.second == NodeType::Act)
                    mChoice = 0;
                else
                    mChoice = -1;
            }

            if (pair.second == NodeType::Dialogue)
            {
                auto* dialogueNode = (DialogueNode*)pair.first;
                for (int32_t i = 0; i < dialogueNode->Prompts.size(); i++)
                {
                    const auto& prompt = dialogueNode->Prompts[i];
                    if (ImGui::Button(prompt.c_str()))
                    {
                        mChoice = i;
                        mDone = true;
                    }
                }
            }
            else if (pair.second == NodeType::Act)
            {
                auto* actNode = (ActNode*)pair.first;
                auto&& [speaker, line] = actNode->Bubbles[mChoice];
                ImGui::Text(speaker == Speaker::NPC ? "NPC:" : "Main Character:");
                ImGui::TextWrapped(line.c_str());

                const bool space = ImGui::IsKeyReleased(ImGuiKey_Space);
                const bool button = ImGui::Button("Next");
                if (space || button)
                {
                    if (++mChoice >= actNode->Bubbles.size())
                        mDone = true;
                }
            }
            else
                mSpeaking = false;
        }
        ImGui::End();
    }
    
    /*auto& io = ImGui::GetIO();

    std::vector<ed::NodeId> selectedNodes;
    std::vector<ed::LinkId> selectedLinks;
    selectedNodes.resize(ed::GetSelectedObjectCount());
    selectedLinks.resize(ed::GetSelectedObjectCount());

    int nodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
    int linkCount = ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

    selectedNodes.resize(nodeCount);
    selectedLinks.resize(linkCount);

    int saveIconWidth = Application_GetTextureWidth(sSaveIcon);
    int saveIconHeight = Application_GetTextureWidth(sSaveIcon);
    int restoreIconWidth = Application_GetTextureWidth(sRestoreIcon);
    int restoreIconHeight = Application_GetTextureWidth(sRestoreIcon);

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing(); ImGui::SameLine();
    ImGui::TextUnformatted("Nodes");
    ImGui::Indent();
    for (auto& node : s_Nodes)
    {
        ImGui::PushID(node.ID.AsPointer());
        auto start = ImGui::GetCursorScreenPos();

        if (const auto progress = GetTouchProgress(node.ID))
        {
            ImGui::GetWindowDrawList()->AddLine(
                start + ImVec2(-8, 0),
                start + ImVec2(-8, ImGui::GetTextLineHeight()),
                IM_COL32(255, 0, 0, 255 - (int)(255 * progress)), 4.0f);
        }

        bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(), node.ID) != selectedNodes.end();
        if (ImGui::Selectable((node.Name + "##" + std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer()))).c_str(), &isSelected))
        {
            if (io.KeyCtrl)
            {
                if (isSelected)
                    ed::SelectNode(node.ID, true);
                else
                    ed::DeselectNode(node.ID);
            }
            else
                ed::SelectNode(node.ID, false);

            ed::NavigateToSelection();
        }
        if (ImGui::IsItemHovered() && !node.State.empty())
            ImGui::SetTooltip("State: %s", node.State.c_str());

        auto id = std::string("(") + std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer())) + ")";
        auto textSize = ImGui::CalcTextSize(id.c_str(), nullptr);
        auto iconPanelPos = start + ImVec2(
            paneWidth - ImGui::GetStyle().FramePadding.x - ImGui::GetStyle().IndentSpacing - saveIconWidth - restoreIconWidth - ImGui::GetStyle().ItemInnerSpacing.x * 1,
            (ImGui::GetTextLineHeight() - saveIconHeight) / 2);
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(iconPanelPos.x - textSize.x - ImGui::GetStyle().ItemInnerSpacing.x, start.y),
            IM_COL32(255, 255, 255, 255), id.c_str(), nullptr);

        auto drawList = ImGui::GetWindowDrawList();
        ImGui::SetCursorScreenPos(iconPanelPos);
        ImGui::SetItemAllowOverlap();
        if (node.SavedState.empty())
        {
            if (ImGui::InvisibleButton("save", ImVec2((float)saveIconWidth, (float)saveIconHeight)))
                node.SavedState = node.State;

            if (ImGui::IsItemActive())
                drawList->AddImage(s_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 96));
            else if (ImGui::IsItemHovered())
                drawList->AddImage(s_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
            else
                drawList->AddImage(s_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 160));
        }
        else
        {
            ImGui::Dummy(ImVec2((float)saveIconWidth, (float)saveIconHeight));
            drawList->AddImage(s_SaveIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 32));
        }

        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::SetItemAllowOverlap();
        if (!node.SavedState.empty())
        {
            if (ImGui::InvisibleButton("restore", ImVec2((float)restoreIconWidth, (float)restoreIconHeight)))
            {
                node.State = node.SavedState;
                ed::RestoreNodeState(node.ID);
                node.SavedState.clear();
            }

            if (ImGui::IsItemActive())
                drawList->AddImage(s_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 96));
            else if (ImGui::IsItemHovered())
                drawList->AddImage(s_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255));
            else
                drawList->AddImage(s_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 160));
        }
        else
        {
            ImGui::Dummy(ImVec2((float)restoreIconWidth, (float)restoreIconHeight));
            drawList->AddImage(s_RestoreIcon, ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 32));
        }

        ImGui::SameLine(0, 0);
        ImGui::SetItemAllowOverlap();
        ImGui::Dummy(ImVec2(0, (float)restoreIconHeight));

        ImGui::PopID();
    }
    ImGui::Unindent();

    static int changeCount = 0;

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing(); ImGui::SameLine();
    ImGui::TextUnformatted("Selection");

    ImGui::BeginHorizontal("Selection Stats", ImVec2(paneWidth, 0));
    ImGui::Text("Changed %d time%s", changeCount, changeCount > 1 ? "s" : "");
    ImGui::Spring();
    if (ImGui::Button("Deselect All"))
        ed::ClearSelection();
    ImGui::EndHorizontal();
    ImGui::Indent();
    for (int i = 0; i < nodeCount; ++i) ImGui::Text("Node (%p)", selectedNodes[i].AsPointer());
    for (int i = 0; i < linkCount; ++i) ImGui::Text("Link (%p)", selectedLinks[i].AsPointer());
    ImGui::Unindent();

    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
        for (auto& link : s_Links)
            ed::Flow(link.ID);

    if (ed::HasSelectionChanged())
        ++changeCount;

    ImGui::EndChild();
    */
}



Scene::Scene(void)
{
    ed::Config config;
    auto* editor = ed::CreateEditor(&config);
    ed::SetCurrentEditor(editor);
    mAllData.emplace_back(CharacterData{ editor });
    mWorkingDataIndex = 0;
}


void ShowStyleEditor(bool* show)
{
    if (!ImGui::Begin("Style", show))
    {
        ImGui::End();
        return;
    }

    auto paneWidth = ImGui::GetContentRegionAvailWidth();

    auto& editorStyle = ed::GetStyle();
    ImGui::BeginHorizontal("Style buttons", ImVec2(paneWidth, 0), 1.0f);
    ImGui::TextUnformatted("Values");
    ImGui::Spring();
    if (ImGui::Button("Reset to defaults"))
        editorStyle = ed::Style();
    ImGui::EndHorizontal();
    ImGui::Spacing();
    ImGui::DragFloat4("Node Padding", &editorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Node Rounding", &editorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Node Border Width", &editorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Hovered Node Border Width", &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Selected Node Border Width", &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Pin Rounding", &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Pin Border Width", &editorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Link Strength", &editorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
    //ImVec2  SourceDirection;
    //ImVec2  TargetDirection;
    ImGui::DragFloat("Scroll Duration", &editorStyle.ScrollDuration, 0.001f, 0.0f, 2.0f);
    ImGui::DragFloat("Flow Marker Distance", &editorStyle.FlowMarkerDistance, 1.0f, 1.0f, 200.0f);
    ImGui::DragFloat("Flow Speed", &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
    ImGui::DragFloat("Flow Duration", &editorStyle.FlowDuration, 0.001f, 0.0f, 5.0f);
    //ImVec2  PivotAlignment;
    //ImVec2  PivotSize;
    //ImVec2  PivotScale;
    //float   PinCorners;
    //float   PinRadius;
    //float   PinArrowSize;
    //float   PinArrowWidth;
    ImGui::DragFloat("Group Rounding", &editorStyle.GroupRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Group Border Width", &editorStyle.GroupBorderWidth, 0.1f, 0.0f, 15.0f);

    ImGui::Separator();

    static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_RGB;
    ImGui::BeginHorizontal("Color Mode", ImVec2(paneWidth, 0), 1.0f);
    ImGui::TextUnformatted("Filter Colors");
    ImGui::Spring();
    ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_RGB);
    ImGui::Spring(0);
    ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_HSV);
    ImGui::Spring(0);
    ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_HEX);
    ImGui::EndHorizontal();

    static ImGuiTextFilter filter;
    filter.Draw("", paneWidth);

    ImGui::Spacing();

    ImGui::PushItemWidth(-160);
    for (int i = 0; i < ed::StyleColor_Count; ++i)
    {
        auto name = ed::GetStyleColorName((ed::StyleColor)i);
        if (!filter.PassFilter(name))
            continue;

        ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

Scene::~Scene(void)
{
    for (const auto& data : mAllData)
        ed::DestroyEditor(data.Editor);
}


void TextWithBackgroundColor(const char* text, const ImVec4& bgColor)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window == nullptr)
        return;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImVec2 textPos = ImGui::GetCursorScreenPos();
    const ImVec2 textSize = ImGui::CalcTextSize(text);

    // Draw the background rectangle
    const ImVec2 end = textPos + textSize + ImVec2{ 10.0f, 0.0f };
    drawList->AddRectFilled(textPos, end, ImGui::GetColorU32(bgColor), 2.0f);

    // Draw the text on top of the background
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);
    ImGui::Text("%s", text);
}

void Scene::Save()
{
    if (!mLastFilepath.empty())
        SceneSerializer{ this }.Serialize(mLastFilepath);
    else
        SaveAs();
}

void Scene::SaveAs()
{
    std::filesystem::path path = CreateFileDialog(FileDialogType::Save);
    if (!path.filename().empty())
    {
        mLastFilepath = (path.replace_extension(".puru")).string();
        Save();
    }
}

void Scene::Open()
{
    std::filesystem::path path = CreateFileDialog(FileDialogType::Open);
    if (!path.empty())
    {
        mLastFilepath = (path.replace_extension(".puru")).string();
        SceneSerializer{ this }.Deserialize(path.string());
    }
}