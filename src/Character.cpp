#include <Character.h>
#include <Components.h>
#include <Nodes.hpp>

#include <imgui_node_editor.h>
#include <Application.h>
#include <algorithm>

#include <SceneSerializer.h>
#include <Random.h>

using namespace ax;

const ed::PinId INVALID_PIN_ID = ed::PinId{ 0 };
static ImTextureID sHeaderBackground;

static bool IsFlavorMatching(Flavor lhs, Flavor rhs);

static bool IsFlavorSame(Flavor lhs, Flavor rhs);

static void AddNewLines(char* text, size_t N = 32);

entt::registry Character::sQuestECS;
size_t Character::sNextID = 0;

[[nodiscard]] ImTextureID& GetHeaderBackground()
{
    return sHeaderBackground;
}

std::string Character::FindSelectedQuestTitle(const gte::uuid& selection)
{
    if (!selection)
        return "Select Quest";

    auto view = sQuestECS.view<AcceptQuestNode>();
    for (auto&& [entityID, quest] : view.each())
        if (quest.UUID == selection)
            return quest.Title;

    return "Select Quest";
}

std::string Character::FindSelectedObjectiveTitle(const gte::uuid& questSelection, const gte::uuid& objectiveSelection)
{
    if (!questSelection || !objectiveSelection)
        return "Select Objective";

    auto view = sQuestECS.view<AcceptQuestNode>();
    for (auto&& [entityID, quest] : view.each())
        if (quest.UUID == questSelection)
            for (const auto& objective : quest.Objectives)
                if (objective.UUID == objectiveSelection)
                    return objective.Title;

    return "Select Objective";
}

void Character::RenderHeader(NodeBuilder& builder, const char* name, const ImColor& color) const
{
    builder.Header(color);

    ImGui::Spring(0);
    ImGui::TextUnformatted(name);
    ImGui::Spring(1);
    ImGui::Dummy({ 0.0f, 28.0f });

    ImGui::Spring(0);
    builder.EndHeader();
}

void Character::RenderInput(NodeBuilder& builder, const Pin& input) const
{
    auto alpha = ImGui::GetStyle().Alpha;
    if (mNewLinkPin && !CanCreateLink(*mNewLinkPin, input) && &input != mNewLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    builder.Input(input.ID);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    DrawPinIcon(input, IsPinLinked(input.ID), (int)(alpha * 255));
    ImGui::Spring(0);
    if (!input.Name.empty())
    {
        ImGui::TextUnformatted(input.Name.c_str());
        ImGui::Spring(0);
    }
    ImGui::PopStyleVar();
    builder.EndInput();
}

void Character::RenderOutput(NodeBuilder& builder, const Pin& output) const
{
    builder.Output(output.ID);
    auto alpha = ImGui::GetStyle().Alpha;
    if (mNewLinkPin && !CanCreateLink(*mNewLinkPin, output) && &output != mNewLinkPin)
        alpha = alpha * (48.0f / 255.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    if (!output.Name.empty())
    {
        ImGui::Spring(0);
        ImGui::TextUnformatted(output.Name.c_str());
    }
    ImGui::Spring(0);
    DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
    ImGui::PopStyleVar();
    builder.EndOutput();
}


void Character::RenderNodes(void)
{
    auto* headerBackground = GetHeaderBackground();
    util::BlueprintNodeBuilder builder(headerBackground, Application_GetTextureWidth(headerBackground), Application_GetTextureHeight(headerBackground));

    {// Special node for enty only
        auto view = mECS.view<Node>();
        for (auto&& [entityID, node] : view.each())
        {
            builder.Begin(node.ID);
            const auto& output = mECS.get<Pin>(entityID);
            builder.Middle();
            ImGui::Spring(1.0f, 0.0f);
            ImGui::TextUnformatted("Entry");
            ImGui::Spring(1.0f, 0.0f);
            RenderOutput(builder, output);
            builder.End();
        }
    }

    {
        RenderVariableNode<bool>(builder);
        RenderVariableNode<int32_t>(builder);
    }

    {
        auto& view = mECS.view<ActNode>();
        for (auto&& [entityID, node] : view.each())
        {

            builder.Begin(node.ID);
            RenderHeader(builder, ActNode::NAME, ActNode::COLOR);
            const auto& pins = mECS.get<InputOutput>(entityID);
            RenderInput(builder, pins.Input);
            

            builder.Middle();
            ImGui::Spring(1, 0);
            ImGui::PushItemWidth(120.0f);
            ImGui::InputText("", node.Title, STR_LENGTH);
            if (ImGui::Button("Edit"))
                mOpenActNode = entityID;
            ImGui::Spring(0, 1);

            RenderOutput(builder, pins.Output);
            builder.End();
        }

        if (Scene::sWindows[Scene::ACT_INDEX] && mOpenActNode != entt::null && mECS.valid(mOpenActNode))
        {
            ed::Suspend();
            ImGui::SetNextWindowSize({ 512, 912 });
            if (ImGui::Begin("Act Node", &Scene::sWindows[Scene::ACT_INDEX], ImGuiWindowFlags_NoResize))
            {
                static constexpr size_t INVALID_INDEX = static_cast<size_t>(-1);
                static constexpr char* typestr[3] = { "Main Character", "NPC", "Internal" };
                auto& node = view.get<ActNode>(mOpenActNode);
                size_t i = 0;
                size_t delIndex = INVALID_INDEX;
                for (auto&& [speaker, line] : node.Bubbles)
                {
                    const std::string btnLabel = "X##" + std::to_string(i);
                    ImGui::PushStyleColor(ImGuiCol_Button, { 0.9f, 0.125f, 0.213f, 1.0f });
                    if (ImGui::Button(btnLabel.c_str(), { 24, 24 }))
                        delIndex = i;
                    ImGui::SameLine();
                    ImGui::PopStyleColor();
                    ImGui::PushItemWidth(128.0f);
                    const std::string comboLabel = std::string("##combo") + std::to_string(i);
                    ImGui::Combo(comboLabel.c_str(), (int32_t*)&speaker, typestr, IM_ARRAYSIZE(typestr)); ImGui::SameLine();
                    ImGui::PopItemWidth();
                    char buffer[4096] = { 0 };
                    memcpy(buffer, line.c_str(), line.size() + 1);
                    const std::string textLabel = std::string("##line") + std::to_string(i);
                    if (ImGui::InputTextMultiline(textLabel.c_str(), buffer, 4096, { -1, 0 }))
                        line = buffer;
                    i++;
                }
                if (ImGui::Button("Add"))
                    node.Bubbles.emplace_back(std::make_pair(Speaker::MainCharacter, ""));
                if (ImGui::Button("Close"))
                    mOpenActNode = entt::null;
                if (delIndex != INVALID_INDEX)
                    node.Bubbles.erase(node.Bubbles.begin() + delIndex);
            }
            ImGui::End();
            ed::Resume();
        }
    }

    {
        auto& view = mECS.view<ForkNode>();
        for (auto&& [entityID, node] : view.each())
        {

            builder.Begin(node.ID);
            RenderHeader(builder, ForkNode::NAME, ForkNode::COLOR);
            const auto& pins = mECS.get<ForkInputOutput>(entityID);

            RenderInput(builder, pins.Input);
            RenderOutput(builder, pins.Outputs[0]);
            RenderOutput(builder, pins.Outputs[1]);
            builder.End();
        }
    }

    {
        static constexpr char* operators[] = { "==", ">", "<", ">=", "<=", "<>" };
        auto& view = mECS.view<BranchNode>();
        for (auto&& [entityID, node] : view.each())
        {
            builder.Begin(node.ID);
            RenderHeader(builder, BranchNode::NAME, BranchNode::COLOR);
            auto& pins = mECS.get<InputOutputs>(entityID);
            RenderInput(builder, pins.Input);

            builder.Middle();
            ImGui::Spring(1, 0);
            int32_t iOperator = -1, iRemove = -1;
            for (int32_t i = 0; i < node.Expressions.size(); i++)
            {
                auto& expression = node.Expressions[i];
                ImGui::PushID(i);
                if (i > 0)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
                    const std::string erase = "X##" + std::to_string(i);
                    ImGui::Dummy({ 0.0f, 0.0f });
                    ImGui::SameLine(-1.0f);
                    ImGui::Dummy({ 1.0f, 4.0f });
                    ImGui::SameLine();
                    if(ImGui::Button(erase.c_str(), { OPERATOR_BUTTON_SIZE.y, OPERATOR_BUTTON_SIZE.y }))
                        iRemove = i;
                    ImGui::PopStyleColor(3);
                    ImGui::SameLine();
                }
                ImGui::TextUnformatted("if"); ImGui::SameLine();
                ImGui::InputText("##name", expression[0].VariableName, STR_LENGTH); ImGui::SameLine();
                const int32_t index = static_cast<int32_t>(expression[0].Operator);
                if (ImGui::Button(operators[index], OPERATOR_BUTTON_SIZE))
                    iOperator = i;
                ImGui::SameLine();
                ImGui::DragScalar("##value", ImGuiDataType_S32, &expression[0].Value, 1.0f); ImGui::SameLine();
                if (ImGui::Button(":", { 0.0f, OPERATOR_BUTTON_SIZE.y }))
                    mOpenExpression = std::make_pair(entityID, i);
                ImGui::PopID();
            }
            //const float width = ImGui::GetContentRegionAvail().x;
            //const float offset = width / 2.0f;
            ImGui::Dummy({ 0.0f, 28.0f });
            ImGui::Dummy({ 0.0f, 0.0f });
            ImGui::SameLine(0.0f, 99.0f);
            const bool pressedAdd = ImGui::Button("+", { 124.0f, 0.0f });
            ImGui::Spring(1, 0);
            for(const auto& output : pins.Outputs)
                RenderOutput(builder, output);
            builder.End();
            ed::Suspend();
            static int32_t indexOperator = -1;
            static entt::entity sEntityID = entt::null;
            if (iOperator != -1)
            {
                ImGui::OpenPopup("ComparisonOperator");
                indexOperator = iOperator;
                iOperator = -1;
                sEntityID = entityID;
            }
            if (sEntityID == entityID && ImGui::BeginPopup("ComparisonOperator"))
            {
                constexpr int32_t size = IM_ARRAYSIZE(operators);
                for (int i = 0; i < size; i++)
                {
                    if (ImGui::MenuItem(operators[i]))
                    {
                        node.Expressions[0][indexOperator].Operator = static_cast<CompareOperator>(i);
                        indexOperator = -1;
                    }
                }
               ImGui::EndPopup();
            }
            ed::Resume();
            if (pressedAdd)
            {
                auto& expression = node.Expressions.emplace_back();
                expression.emplace_back();
                pins.Outputs.emplace(pins.Outputs.end() - 1, GetNextID(), "then", PinKind::Output);
            }
            if (iRemove >= 0)
            {
                node.Expressions.erase(node.Expressions.begin() + iRemove);
                const auto it = pins.Outputs.begin() + iRemove;
                auto links = mECS.view<Link>();
                for (auto&& [entityID, link] : links.each())
                {
                    if (link.StartPinID == it->ID)
                    {
                        mECS.remove<Link>(entityID);
                        break;
                    }
                }
                pins.Outputs.erase(it);
            }
        }

        auto [entityID, index] = mOpenExpression;
        if (auto* nodeptr = mECS.try_get<BranchNode>(entityID);
            nodeptr != nullptr && index >= 0 && index < nodeptr->Expressions.size()
            && Scene::sWindows[Scene::EXPRESSION_INDEX])
        {
            ed::Suspend();
            if (ImGui::Begin("Expression", &Scene::sWindows[Scene::EXPRESSION_INDEX]))
            {
                auto& expression = nodeptr->Expressions[index];
                int32_t iRemove = -1;
                for (int i = 0; i < expression.size(); i++)
                {
                    auto& condition = expression[i];
                    ImGui::PushID(i);
                    if (i > 0) ImGui::TextUnformatted("and");
                    else ImGui::Dummy({ 28.0f, 0.0f });
                    ImGui::SameLine();
                    ImGui::PushItemWidth(128.0f);
                    ImGui::InputText("##name", condition.VariableName, STR_LENGTH); ImGui::SameLine();
                    ImGui::PopItemWidth();
                    int32_t iOp = static_cast<int32_t>(condition.Operator);
                    ImGui::PushItemWidth(64.0f);
                    if (ImGui::Combo("##operator", &iOp, operators, ARRAYSIZE(operators)))
                        condition.Operator = static_cast<CompareOperator>(iOp);
                    ImGui::PopItemWidth();
                    ImGui::SameLine();
                    ImGui::PushItemWidth(128.0f);
                    ImGui::DragScalar("##value", ImGuiDataType_S32, &condition.Value, 1.0f);
                    ImGui::PopItemWidth();
                    if (i > 0)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
                        const std::string erase = "X##" + std::to_string(i);
                        ImGui::SameLine();
                        if (ImGui::Button(erase.c_str(), { OPERATOR_BUTTON_SIZE.y, OPERATOR_BUTTON_SIZE.y }))
                            iRemove = i;
                        ImGui::PopStyleColor(3);
                    }
                    ImGui::PopID();
                }
                if (ImGui::Button("Add"))
                    expression.emplace_back();
                if (ImGui::Button("Close"))
                    mOpenExpression = std::make_pair(entt::null, -1);
                if (iRemove >= 0)
                    expression.erase(expression.begin() + iRemove);
            }
            ImGui::End();
            ed::Resume();
        }
    }

    {
        auto view = mECS.view<DialogueNode>();
        for (auto&& [entityID, node] : view.each())
        {
            builder.Begin(node.ID);
            RenderHeader(builder, DialogueNode::NAME, DialogueNode::COLOR);
            auto& pins = mECS.get<InputOutputs>(entityID);
            RenderInput(builder, pins.Input);

            builder.Middle();
            ImGui::Spring(1, 0);
            int32_t i = 0;
            for (auto& prompt : node.Prompts)
            {
                ImGui::PushID(i++);
                char buffer[128] = { 0 };
                memcpy(buffer, prompt.c_str(), prompt.size());
                ImGui::PushItemWidth(124.0f);
                if (ImGui::InputText("##prompt", buffer, 128))
                    prompt = std::string(buffer);
                ImGui::PopItemWidth();
                ImGui::PopID();
            }
            const bool pressedAdd = ImGui::Button("+", { 124.0f, 0.0f });
            ImGui::Spring(1, 0);

            for (const auto& output : pins.Outputs)
                RenderOutput(builder, output);
            builder.End();

            ed::Suspend();
            if (pressedAdd)
            {
                node.Prompts.emplace_back("Another one");
                pins.Outputs.emplace_back(GetNextID(), "", PinKind::Output);
            }
            ed::Resume();
        }
    }

    {
        auto view = mECS.view<FlavorMatchNode>();
        for (auto&& [entityID, node] : view.each())
        {
            builder.Begin(node.ID);
            RenderHeader(builder, FlavorMatchNode::NAME, FlavorMatchNode::COLOR);
            auto& pins = mECS.get<ForkInputOutput>(entityID);
            RenderInput(builder, pins.Input);
            for (const auto& output : pins.Outputs)
                RenderOutput(builder, output);
            builder.End();
        }
    }

    {
        auto view = mECS.view<FlavorCheckNode>();
        for (auto&& [entityID, node] : view.each())
        {
            builder.Begin(node.ID);
            const auto name = std::string(FlavorCheckNode::NAME) + "(" + (node.CheckingNPC ? "NPC" : "Main Character") + ")";
            RenderHeader(builder, name.c_str(), FlavorCheckNode::COLOR);
            auto& pins = mECS.get<InputOutputs>(entityID);

            RenderInput(builder, pins.Input);
            for (const auto& output : pins.Outputs)
                RenderOutput(builder, output);
            builder.End();
        }
    }

    {
        auto view = mECS.view<DiceNode>();
        for (auto&& [entityID, node] : view.each())
        {
            builder.Begin(node.ID);
            RenderHeader(builder, DiceNode::NAME, DiceNode::COLOR);
            auto& pins = mECS.get<InputOutputs>(entityID);
            RenderInput(builder, pins.Input);

            builder.Middle();
            ImGui::Spring(1, 0);
            int32_t iRemove = -1;
            for (int32_t i = 0; i < pins.Outputs.size(); i++)
            {
                ImGui::PushID(i);
                if (i > 1)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
                    const std::string erase = "X##" + std::to_string(i);
                    ImGui::Dummy({ 0.0f, 0.0f });
                    ImGui::SameLine(-1.0f);
                    ImGui::Dummy({ 1.0f, 4.0f });
                    ImGui::SameLine();
                    if (ImGui::Button(erase.c_str(), { OPERATOR_BUTTON_SIZE.y, OPERATOR_BUTTON_SIZE.y }))
                        iRemove = i;
                    ImGui::PopStyleColor(3);
                    ImGui::SameLine();
                }
                ImGui::Dummy({ 0.0f, 28.0f });
                ImGui::PopID();
            }
            ImGui::Dummy({ 0.0f, 0.0f });
            const bool pressedAdd = ImGui::Button("+", { 36.0f, 0.0f });
            ImGui::Spring(1, 0);
            for (const auto& output : pins.Outputs)
                RenderOutput(builder, output);
            builder.End();
            ed::Suspend();
            if (pressedAdd)
                pins.Outputs.emplace_back(GetNextID(), PinKind::Output);
            if (iRemove > 1)
            {
                const auto it = pins.Outputs.begin() + iRemove;
                auto links = mECS.view<Link>();
                for (auto&& [entityID, link] : links.each())
                {
                    if (link.StartPinID == it->ID)
                    {
                        mECS.remove<Link>(entityID);
                        break;
                    }
                }
                pins.Outputs.erase(it);
            }
            ed::Resume();
        }
    }
    
    {
        auto view = sQuestECS.view<AcceptQuestNode, InputOutput>();
        for (auto&& [entityID, node, pins] : view.each())
        {
            if (node.Owner != mID)
                continue;

            builder.Begin(node.ID);
            RenderHeader(builder, AcceptQuestNode::NAME, AcceptQuestNode::COLOR);
            RenderInput(builder, pins.Input);

            builder.Middle();
            ImGui::Spring(1, 0);
            ImGui::InputText("##Title", node.Title, 64);
            if (ImGui::Button("Edit"))
                mOpenAcceptQuest = entityID;
            ImGui::Spring(1, 0);
            RenderOutput(builder, pins.Output);
            builder.End();
        }

        if (Scene::sWindows[Scene::QUEST_INDEX]
            && mOpenAcceptQuest != entt::null
            && sQuestECS.valid(mOpenAcceptQuest))
        {
            auto& node = sQuestECS.get<AcceptQuestNode>(mOpenAcceptQuest);
            ed::Suspend();
            if (ImGui::Begin("Quest Node", &Scene::sWindows[Scene::QUEST_INDEX]))
            {
                ImGui::TextUnformatted("Description:"); ImGui::SameLine();
                AddNewLines(node.Description);
                ImGui::InputTextMultiline("##Description", node.Description, 512);
                ImGui::TextUnformatted("Objectives:");
                int32_t iRemove = -1;
                for (int32_t i = 0; i < node.Objectives.size(); i++)
                {
                    auto& objective = node.Objectives[i];
                    ImGui::PushID(i);
                    ImGui::Dummy({ 0.0f, 0.0f }); ImGui::SameLine(0.0f, ImGui::GetContentRegionAvail().x - 30.0f);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
                    if (ImGui::Button("X", { OPERATOR_BUTTON_SIZE.y, OPERATOR_BUTTON_SIZE.y }))
                        iRemove = i;
                    ImGui::PopStyleColor(3);

                    ImGui::TextUnformatted("Is Optional:"); ImGui::SameLine();
                    ImGui::Checkbox("##Optional", &objective.IsOptional);
                    ImGui::InputText("##ObjectiveTitle", objective.Title, STR_LENGTH);
                    AddNewLines(objective.Description);
                    ImGui::InputTextMultiline("##ObjectiveDescription", objective.Description, 512);
                    ImGui::Separator();
                    ImGui::PopID();
                }
                if (iRemove != -1)
                    node.Objectives.erase(node.Objectives.begin() + iRemove);
                if (ImGui::Button("Add"))
                    node.Objectives.emplace_back();
                if (ImGui::Button("Close"))
                    mOpenAcceptQuest = entt::null;
            }
            ImGui::End();
            ed::Resume();
        }
    }

    {
        auto view = mECS.view<ReturnQuestNode>();
        static entt::entity sSelectingQuest = entt::null;
        for (auto&& [entityID, node] : view.each())
        {
            builder.Begin(node.ID);
            RenderHeader(builder, ReturnQuestNode::NAME, ReturnQuestNode::COLOR);
            auto& pins = mECS.get<InputOutput>(entityID);
            RenderInput(builder, pins.Input);
            builder.Middle();
            ImGui::Spring(1, 0);
            const auto selection = FindSelectedQuestTitle(node.QuestID);
            const bool pressed = ImGui::Button(selection.c_str());
            if (ImGui::RadioButton("Succeed", node.Succeed)) node.Succeed = true;
            if (ImGui::RadioButton("Failed", !node.Succeed)) node.Succeed = false;
            ImGui::Spring(1, 0);
            RenderOutput(builder, pins.Output);
            builder.End();

            if (pressed)
            {
                ImGui::OpenPopup("Select Quest");
                sSelectingQuest = entityID;
            }
            ed::Suspend();
            if (sSelectingQuest == entityID && ImGui::BeginPopup("Select Quest"))
            {
                auto quests = sQuestECS.view<AcceptQuestNode>();
                for (auto&& [nodeId, quest] : quests.each())
                {
                    if (ImGui::MenuItem(quest.Title, nullptr, node.QuestID == quest.UUID))
                    {
                        node.QuestID = quest.UUID;
                        sSelectingQuest = entt::null;
                    }
                }
                ImGui::EndPopup();
            }
            ed::Resume();
        }
    }

    {
        auto view = mECS.view<ObjectiveNode>();
        static entt::entity sSelectingQuest = entt::null;
        static entt::entity sSelectingObjective = entt::null;
        for (auto&& [entityID, node] : view.each())
        {
            builder.Begin(node.ID);
            RenderHeader(builder, ObjectiveNode::NAME, ObjectiveNode::COLOR);
            auto& pins = mECS.get<InputOutput>(entityID);
            RenderInput(builder, pins.Input);
            builder.Middle();
            ImGui::Spring(1, 0);
            const auto questSelection = FindSelectedQuestTitle(node.QuestID);
            const bool pressedQuest = ImGui::Button(questSelection.c_str());
            const auto objectiveSelection = FindSelectedObjectiveTitle(node.QuestID, node.ObjectiveID);
            const bool pressedObjective = ImGui::Button(objectiveSelection.c_str());
            ImGui::Dummy({ 0.0f, 0.0f });
            if (ImGui::RadioButton("Succeed", node.Succeed)) node.Succeed = true;
            if (ImGui::RadioButton("Failed", !node.Succeed)) node.Succeed = false;
            ImGui::Spring(1, 0);
            RenderOutput(builder, pins.Output);
            builder.End();

            if (pressedQuest)
            {
                ImGui::OpenPopup("Select Quest");
                sSelectingQuest = entityID;
            }
            else if (pressedObjective)
            {
                ImGui::OpenPopup("Select Objective");
                sSelectingObjective = entityID;
            }
            ed::Suspend();
            if (sSelectingQuest == entityID && ImGui::BeginPopup("Select Quest"))
            {
                auto quests = sQuestECS.view<AcceptQuestNode>();
                for (auto&& [nodeId, quest] : quests.each())
                {
                    if (ImGui::MenuItem(quest.Title, nullptr, node.QuestID == quest.UUID))
                    {
                        node.QuestID = quest.UUID;
                        sSelectingQuest = entt::null;
                    }
                }
                ImGui::EndPopup();
            }
            else if (sSelectingObjective == entityID && ImGui::BeginPopup("Select Objective"))
            {
                auto quests = sQuestECS.view<AcceptQuestNode>();
                for (auto&& [nodeId, quest] : quests.each())
                {
                    if (quest.UUID != node.QuestID)
                        continue;
                    for (auto& objective : quest.Objectives)
                    {
                        if (ImGui::MenuItem(objective.Title, nullptr, node.ObjectiveID == objective.UUID))
                        {
                            node.ObjectiveID = objective.UUID;
                            sSelectingQuest = entt::null;
                        }
                    }
                }
                ImGui::EndPopup();
            }
            ed::Resume();
        }
    }

    {
        auto view = mECS.view<CommentNode>();
        static entt::entity editingComment = entt::null;
        for (auto&& [entityID, node] : view.each())
        {
            constexpr float commentAplha = 0.75f;
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAplha);
            ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
            ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
            ed::BeginNode(node.ID);
            ImGui::PushID(node.ID.AsPointer());
            ImGui::BeginVertical("content");
            ImGui::BeginHorizontal("horizontal");
            ImGui::Spring(1);
            if (editingComment == entityID)
            {
                char buffer[4096];
                strcpy(buffer, node.Comment.c_str());
                if (ImGui::InputText("##Comment", buffer, 4096))
                    node.Comment = std::string(buffer);
                if (ImGui::IsMouseClicked(0) && !ImGui::IsItemHovered())
                    editingComment = entt::null;
            }
            else
            {
                ImGui::TextUnformatted(node.Comment.c_str());
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    editingComment = entityID;
            }
            ImGui::Spring(1);
            ImGui::EndHorizontal();
            ed::Group(node.Size);
            ImGui::EndVertical();
            ImGui::PopID();
            ed::EndNode();
            ed::PopStyleColor(2);
            ImGui::PopStyleVar();

            if (ed::BeginGroupHint(node.ID))
            {
                auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

                //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

                auto min = ed::GetGroupMin();
                //auto max = ed::GetGroupMax();

                ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
                ImGui::BeginGroup();
                ImGui::TextUnformatted(node.Comment.c_str());
                ImGui::EndGroup();

                auto drawList = ed::GetHintBackgroundDrawList();

                auto hintBounds = ImGui_GetItemRect();
                auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

                drawList->AddRectFilled(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

                drawList->AddRect(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

                //ImGui::PopStyleVar();
            }
            ed::EndGroupHint();
        }
    }
}

[[nodiscard]] Pin Character::FindPin(entt::entity entityID, ed::PinId pinId) const
{
    if (auto* pin = mECS.try_get<Pin>(entityID))
        if (pin->ID == pinId)
            return *pin;
    if (auto* pins = mECS.try_get<InputOutput>(entityID))
    {
        if (pins->Input.ID == pinId)
            return pins->Input;
        if (pins->Output.ID == pinId)
            return pins->Output;
    }

    if (auto* pins = mECS.try_get<ForkInputOutput>(entityID))
    {
        if (pins->Input.ID == pinId)
            return pins->Input;
        if (pins->Outputs[0].ID == pinId)
            return pins->Outputs[0];
        if (pins->Outputs[1].ID == pinId)
            return pins->Outputs[1];
    }

    if (auto* pins = mECS.try_get<InputOutputs>(entityID))
    {
        if (pins->Input.ID == pinId)
            return pins->Input;
        for (auto& output : pins->Outputs)
            if (output.ID == pinId)
                return output;
    }


    return {};
}

//[[nodiscard]] const Pin Character::FindPin(entt::entity entityID, ed::PinId pinId) const { return FindPin(entityID, pinId); }

[[nodiscard]]Link* Character::FindLink(ed::LinkId linkID)
{
    auto view = mECS.view<Link>();
    for (auto&& [enitityID, link] : view.each())
        if (link.ID == linkID)
            return &link;
    return nullptr;
}

[[nodiscard]] Link* Character::FindLink(ed::PinId pinID)
{
    auto view = mECS.view<Link>();
    for (auto&& [entityID, link] : view.each())
        if (link.StartPinID == pinID || link.EndPinID == pinID)
            return &link;
    return nullptr;
}

bool Character::IsPinLinked(ed::PinId id) const
{
    auto view = mECS.view<Link>();
    for (auto&& [entityID, link] : view.each())
        if (link.StartPinID == id || link.EndPinID == id)
            return true;
    return false;
}

bool Character::CanCreateLink(const Pin& a, const Pin& b) const
{
    if (a.ID == INVALID_PIN_ID || b.ID == INVALID_PIN_ID ||
        a.Kind == b.Kind || a.Type != b.Type
    )
        return false;

    {
        auto view = mECS.view<InputOutput>();
        for (auto&& [entityID, pins] : view.each())
            if ((pins.Input.ID == a.ID && pins.Output.ID == b.ID)
                || (pins.Input.ID == b.ID && pins.Output.ID == a.ID)
            )
                return false;
    }

    {
        auto view = mECS.view<ForkInputOutput>();
        for (auto&& [entityID, pins] : view.each())
            if ((pins.Input.ID == a.ID && pins.Outputs[0].ID == b.ID)
                || (pins.Input.ID == a.ID && pins.Outputs[1].ID == b.ID)
                || (pins.Input.ID == b.ID && pins.Outputs[0].ID == a.ID)
                || (pins.Input.ID == b.ID && pins.Outputs[1].ID == a.ID)
            )
                return false;
    }

    {
        auto view = mECS.view<InputOutputs>();
        for (auto&& [entityID, pins] : view.each())
            for (auto& output : pins.Outputs)
                if((pins.Input.ID == a.ID && output.ID == b.ID)
                    || (pins.Input.ID == b.ID && output.ID == a.ID)
                )
                    return false;
    }
    return true;
}

void Character::UpdateTouch(void)
{
    const auto deltaTime = ImGui::GetIO().DeltaTime;
    for (auto& entry : mNodeTouchTime)
    {
        if (entry.second > 0.0f)
            entry.second -= deltaTime;
    }
}

float Character::GetTouchProgress(ed::NodeId id)
{
    auto it = mNodeTouchTime.find(id);
    if (it != mNodeTouchTime.end() && it->second > 0.0f)
        return (sTouchTime - it->second) / sTouchTime;
    else
        return 0.0f;
}

bool Character::Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size)
{
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

[[nodiscard]] entt::entity Character::SpawnBranchNode(void)
{
    const auto entityID = mECS.create();
    auto& node = mECS.emplace<BranchNode>(entityID, GetNextID());
    auto& expression = node.Expressions.emplace_back();
    expression.emplace_back();
    auto& pins = mECS.emplace<InputOutputs>(entityID);
    pins.Input = { GetNextID(), "", PinType::Flow };
    pins.Input.Kind = PinKind::Input;

    Pin& pin0 = pins.Outputs.emplace_back(GetNextID(), "then", PinType::Flow);
    pin0.Kind = PinKind::Output;
    
    Pin& pin1 = pins.Outputs.emplace_back(GetNextID(), "else", PinType::Flow);
    pin1.Kind = PinKind::Output;

    return entityID;
}

[[nodiscard]] entt::entity Character::SpawnForkNode(void)
{
    const auto entityID = mECS.create();
    auto& node = mECS.emplace<ForkNode>(entityID, GetNextID());

    auto& pins = mECS.emplace<ForkInputOutput>(entityID);
    pins.Input = { GetNextID(), "", PinType::Flow };
    pins.Input.Kind = PinKind::Input;

    pins.Outputs[0] = { GetNextID(), "Others", PinType::Flow };
    pins.Outputs[0].Kind = PinKind::Output;
    pins.Outputs[1] = { GetNextID(), "First", PinType::Flow };
    pins.Outputs[1].Kind = PinKind::Output;

    return entityID;
}



[[nodiscard]] entt::entity Character::SpawnActNode()
{
    const auto entityID = mECS.create();
    auto& node = mECS.emplace<ActNode>(entityID, GetNextID());

    auto& pins = mECS.emplace<InputOutput>(entityID);
    pins.Input = { GetNextID(), "", PinType::Flow };
    pins.Input.Kind = PinKind::Input;

    pins.Output = { GetNextID(), "", PinType::Flow };
    pins.Output.Kind = PinKind::Output;

    return entityID;
}

[[nodiscard]] entt::entity Character::SpawnDialogueNode(void)
{
    const auto entityID = mECS.create();
    auto& node = mECS.emplace<DialogueNode>(entityID, GetNextID());
    node.Prompts.emplace_back("Something");
    node.Prompts.emplace_back("Something else");

    auto& pins = mECS.emplace<InputOutputs>(entityID);
    pins.Input = { GetNextID(), "", PinType::Flow };
    pins.Input.Kind = PinKind::Input;

    pins.Outputs.emplace_back(GetNextID(), "", PinKind::Output, PinType::Flow);
    pins.Outputs.emplace_back(GetNextID(), "", PinKind::Output, PinType::Flow);

    return entityID;
}

entt::entity Character::SpawnFlavorMatch(void)
{
    const auto entityID = mECS.create();
    auto& node = mECS.emplace<FlavorMatchNode>(entityID, GetNextID());
    
    auto& pins = mECS.emplace<ForkInputOutput>(entityID);
    pins.Input = { GetNextID(), "", PinKind::Input };
    pins.Outputs[0] = { GetNextID(), "Flavor matching", PinKind::Output };
    pins.Outputs[1] = { GetNextID(), "else", PinKind::Output };
    return entityID;
}

entt::entity Character::SpawnFlavorCheck(bool forNpc)
{
    const auto entityID = mECS.create();
    auto& node = mECS.emplace<FlavorCheckNode>(entityID, GetNextID(), forNpc);

    auto& pins = mECS.emplace<InputOutputs>(entityID);
    pins.Input = { GetNextID(), "", PinKind::Input };

    pins.Outputs.emplace_back(GetNextID(), "Bitter", PinKind::Output);
    pins.Outputs.emplace_back(GetNextID(), "Salty", PinKind::Output);
    pins.Outputs.emplace_back(GetNextID(), "Sour", PinKind::Output);
    pins.Outputs.emplace_back(GetNextID(), "Sweet", PinKind::Output);
    pins.Outputs.emplace_back(GetNextID(), "Neutral", PinKind::Output);
    return entityID;
}

entt::entity Character::SpawnDiceNode(void)
{
    const auto entityID = mECS.create();
    auto& node = mECS.emplace<DiceNode>(entityID, GetNextID());

    auto& pins = mECS.emplace<InputOutputs>(entityID);
    pins.Input = { GetNextID(), "", PinKind::Input };
    pins.Outputs.emplace_back(GetNextID(), PinKind::Output);
    pins.Outputs.emplace_back(GetNextID(), PinKind::Output);
    return entityID;
}

entt::entity Character::SpawnAcceptQuestNode(void)
{
    const auto entityID = sQuestECS.create();
    auto& node = sQuestECS.emplace<AcceptQuestNode>(entityID, GetNextID());
    node.UUID = gte::uuid::Create();
    node.Owner = mID;

    auto& pins = sQuestECS.emplace<InputOutput>(entityID);
    pins.Input = { GetNextID(), "", PinKind::Input };
    pins.Output = { GetNextID(), "", PinKind::Output };

    return entityID;
}

entt::entity Character::SpawnReturnQuestNode(void)
{

    const auto entityID = mECS.create();
    auto& node = mECS.emplace<ReturnQuestNode>(entityID, GetNextID());
    
    auto& pins = mECS.emplace<InputOutput>(entityID);
    pins.Input = { GetNextID(), "", PinKind::Input };
    pins.Output = { GetNextID(), "", PinKind::Output };

    return entityID;
}

entt::entity Character::SpawnObjectiveNode(void)
{
    const auto entityID = mECS.create();
    auto& node = mECS.emplace<ObjectiveNode>(entityID, GetNextID());

    auto& pins = mECS.emplace<InputOutput>(entityID);
    pins.Input = { GetNextID(), "", PinKind::Input };
    pins.Output = { GetNextID(), "", PinKind::Output };

    return entityID;
}

entt::entity Character::SpawnCommentNode(void)
{
    const auto entityID = mECS.create();
    auto& node = mECS.emplace<CommentNode>(entityID, GetNextID());
    node.Comment = "Your comment";
    return entityID;
}

[[nodiscard]] entt::entity Character::FindEntity(ed::PinId pinId) const
{
    {
        auto view = mECS.view<Pin>();
        for (auto&& [entityID, pin] : view.each())
            if (pin.ID == pinId)
                return entityID;
    }
    
    {
        auto view = mECS.view<InputOutput>();
        for (auto&& [entityID, node] : view.each())
            if (node.Input.ID == pinId || node.Output.ID == pinId)
                return entityID;
    }
    
    {
        auto view = mECS.view<ForkInputOutput>();
        for (auto&& [entityID, node] : view.each())
            if (node.Input.ID == pinId ||
                node.Outputs[0].ID == pinId ||
                node.Outputs[1].ID == pinId
            )
                return entityID;
    }
    
    {
        auto view = mECS.view<InputOutputs>();
        for (auto&& [entityID, node] : view.each())
        {
            if (node.Input.ID == pinId)
                return entityID;

            for (const auto& pin : node.Outputs)
                if (pin.ID == pinId)
                    return entityID;
        }
    }
    
    return entt::null;
}

[[nodiscard]] int32_t Character::GetEntryNodeID(void)
{
    auto view = mECS.view<Node>();
    for (auto&& [entityID, node] : view.each())
        return (int32_t)node.ID.AsPointer();
    return 0;
}


[[nodiscard]] entt::entity Character::FindEntity(ed::NodeId nodeId) const
{
    if (auto entityID = SearchEntity<Node>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<VariableNode<bool>>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<VariableNode<int32_t>>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<ActNode>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<ForkNode>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<BranchNode>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<DialogueNode>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<FlavorMatchNode>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<FlavorCheckNode>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<DiceNode>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<CommentNode>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<AcceptQuestNode>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<ReturnQuestNode>(nodeId); entityID != entt::null)
        return entityID;
    if (auto entityID = SearchEntity<ObjectiveNode>(nodeId); entityID != entt::null)
        return entityID;
    return entt::null;
}

void Character::HandleInput(void)
{
#if 1
    auto openPopupPosition = ImGui::GetMousePos();
    ed::Suspend();
    if (ed::ShowNodeContextMenu(&mContextNodeId))
        ImGui::OpenPopup("Node Context Menu");
    else if (ed::ShowPinContextMenu(&mContextPinId))
        ImGui::OpenPopup("Pin Context Menu");
    else if (ed::ShowLinkContextMenu(&mContextLinkId))
        ImGui::OpenPopup("Link Context Menu");
    else if (ed::ShowBackgroundContextMenu())
    {
        ImGui::OpenPopup("Create New Node");
        mNewNodeLinkPin = nullptr;
    }
    ed::Resume();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Node Context Menu"))
    {
        auto entityID = FindEntity(mContextNodeId);
        Node* node = nullptr;
        if(auto* nodeptr = mECS.try_get<VariableNode<bool>>(entityID))
            node = nodeptr;
        if (auto* nodeptr = mECS.try_get<VariableNode<int32_t>>(entityID))
            node = nodeptr;
        else if (auto* nodeptr = mECS.try_get<ActNode>(entityID))
            node = nodeptr;
        else if (auto* nodeptr = mECS.try_get<ForkNode>(entityID))
            node = nodeptr;
        else if (auto* nodeptr = mECS.try_get<BranchNode>(entityID))
            node = nodeptr;
        else if (auto* nodeptr = mECS.try_get<FlavorCheckNode>(entityID))
            node = nodeptr;
        else if (auto* nodeptr = mECS.try_get<FlavorMatchNode>(entityID))
            node = nodeptr;
        else if (auto* nodeptr = mECS.try_get<DialogueNode>(entityID))
            node = nodeptr;
        else if (auto* nodeptr = mECS.try_get<DiceNode>(entityID))
            node = nodeptr;
        else if (auto* nodeptr = sQuestECS.try_get<AcceptQuestNode>(entityID))
            node = nodeptr;
        else if (auto* nodeptr = mECS.try_get<ReturnQuestNode>(entityID))
            node = nodeptr;
        else if (auto* nodeptr = mECS.try_get<ObjectiveNode>(entityID))
            node = nodeptr;
        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();
        if (node)
        {
            ImGui::Text("ID: %p", node->ID.AsPointer());
            ImGui::Text("Type: %s", "Blueprint");// : (node->Type == NodeKind::Tree ? "Tree" : "Comment"));
            //ImGui::Text("Inputs: %d", (int)node->Inputs.size());
            //ImGui::Text("Outputs: %d", (int)node->Outputs.size());
        }
        else
            ImGui::Text("Unknown node: %p", mContextNodeId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteNode(mContextNodeId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Pin Context Menu"))
    {
#if 0
        auto pin = FindPin(mContextPinId);

        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();
        if (pin)
        {
            ImGui::Text("ID: %p", pin->ID.AsPointer());
            if (pin->Node)
                ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
            else
                ImGui::Text("Node: %s", "<none>");
        }
        else
            ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

        ImGui::EndPopup();
#endif
    }

    if (ImGui::BeginPopup("Link Context Menu"))
    {
#if 1
        auto link = FindLink(mContextLinkId);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();
        if (link)
        {
            ImGui::Text("ID: %p", link->ID.AsPointer());
            ImGui::Text("From: %p", link->StartPinID.AsPointer());
            ImGui::Text("To: %p", link->EndPinID.AsPointer());
        }
        else
            ImGui::Text("Unknown link: %p", link);
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteLink(mContextLinkId);
        ImGui::EndPopup();
#endif
    }

    if (ImGui::BeginPopup("Create New Node"))
    {
        auto newNodePostion = openPopupPosition;
        //ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

        //auto drawList = ImGui::GetWindowDrawList();
        //drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);
        entt::entity entityID = entt::null;
        ed::PinId endPinId = { 0 };
        if (ImGui::MenuItem("Act"))
        {
            entityID = SpawnActNode();
            endPinId = mECS.get<InputOutput>(entityID).Input.ID;
        }
        if (ImGui::MenuItem("Branch"))
        {
            entityID = SpawnBranchNode();
            endPinId = mECS.get<InputOutputs>(entityID).Input.ID;
        }
        if (ImGui::MenuItem("Dialogue"))
        {
            entityID = SpawnDialogueNode();
            endPinId = mECS.get<InputOutputs>(entityID).Input.ID;
        }
        if (ImGui::MenuItem("Fork"))
        {
            entityID = SpawnForkNode();
            endPinId = mECS.get<ForkInputOutput>(entityID).Input.ID;
        }
        if (ImGui::MenuItem("Dice"))
        {
            entityID = SpawnDiceNode();
            endPinId = mECS.get<InputOutputs>(entityID).Input.ID;
        }
        if (ImGui::MenuItem("Comment"))
            SpawnCommentNode();
        if (ImGui::BeginMenu("Set variable"))
        {
            if (ImGui::MenuItem("Boolean"))
            {
                entityID = SpawnSetVariableNode<bool>();
                endPinId = mECS.get<InputOutput>(entityID).Input.ID;
            }
            if (ImGui::MenuItem("Integer"))
            {
                entityID = SpawnSetVariableNode<int32_t>();
                endPinId = mECS.get<InputOutput>(entityID).Input.ID;
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Flavor Match"))
        {
            entityID = SpawnFlavorMatch();
            endPinId = mECS.get<ForkInputOutput>(entityID).Input.ID;
        }
        if (ImGui::BeginMenu("Flavor Check"))
        {
            if (ImGui::MenuItem("Main Character"))
            {
                entityID = SpawnFlavorCheck(false);
                endPinId = mECS.get<InputOutputs>(entityID).Input.ID;
            }
            if(ImGui::MenuItem("NPC"))
            {
                entityID = SpawnFlavorCheck(true);
                endPinId = mECS.get<InputOutputs>(entityID).Input.ID;
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::BeginMenu("Quests"))
        {
            if (ImGui::MenuItem("Accept Quest"))
            {
                entityID = SpawnAcceptQuestNode();
                endPinId = sQuestECS.get<InputOutput>(entityID).Input.ID;
            }
            if (ImGui::MenuItem("Return Quest"))
            {

                entityID = SpawnReturnQuestNode();
                endPinId = mECS.get<InputOutput>(entityID).Input.ID;
            }
            if (ImGui::MenuItem("Objective"))
            {

                entityID = SpawnObjectiveNode();
                endPinId = mECS.get<InputOutput>(entityID).Input.ID;
            }
            ImGui::EndMenu();
        }
        if (entityID != entt::null)
        {
            mCreateNewNode = false;
            auto* node = FindNodes(AnyNode{}, entityID);
            ed::SetNodePosition(node->ID, newNodePostion);

            if (auto startPin = mNewNodeLinkPin)
            {
                auto linkEntityID = mECS.create();
                if (startPin->Kind == PinKind::Input)
                    std::swap(startPin->ID, endPinId);
                auto& link = mECS.emplace<Link>(linkEntityID, GetNextID(), startPin->ID, endPinId);
                
                //for (auto& pin : pins)
                //{
                //    if (CanCreateLink(*startPin, &pin))
                //    {
                //        auto endPin = &pin;
                //        if (startPin->Kind == PinKind::Input)
                //            std::swap(startPin, endPin);
                //
                //        auto& links = GetLinks();
                //        links.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
                //        links.back().Color = GetIconColor(startPin->Type);
                //
                //        break;
                //    }
                //}
            }
        }

        ImGui::EndPopup();
    }
    else
        mCreateNewNode = false;
    ImGui::PopStyleVar();
    ed::Resume();
#endif
}

void Character::SetupVariables(StateMachine& stateMachine) const
{
    {
        auto view = mECS.view<VariableNode<bool>>();
        for (auto&& [entityID, node] : view.each())
            stateMachine.SetValue(node.VariableName, false);
    }

    {
        auto view = mECS.view<VariableNode<int32_t>>();
        for (auto&& [entityID, node] : view.each())
            stateMachine.SetValue(node.VariableName, 0);
    }

    {
        auto view = mECS.view<ForkNode>();
        for (auto&& [entityID, node] : view.each())
            stateMachine.SetFork(node.UUID.str(), false);
    }
}

[[nodiscard]] NodeType Character::FindNodeType(entt::entity entityID)
{
    if (auto* nodeptr = mECS.try_get<Node>(entityID))
        return NodeType::Entry;
    else if (auto* nodeptr = mECS.try_get<VariableNode<bool>>(entityID))
        return NodeType::BoolVariable;
    else if (auto* nodeptr = mECS.try_get<VariableNode<int32_t>>(entityID))
        return NodeType::IntVariable;
    else if (auto* nodeptr = mECS.try_get<ActNode>(entityID))
        return NodeType::Act;
    else if (auto* nodeptr = mECS.try_get<ForkNode>(entityID))
        return NodeType::Fork;
    else if (auto* nodeptr = mECS.try_get<BranchNode>(entityID))
        return NodeType::Branch;
    else if (auto* nodeptr = mECS.try_get<FlavorCheckNode>(entityID))
        return NodeType::FlavorCheck;
    else if (auto* nodeptr = mECS.try_get<FlavorMatchNode>(entityID))
        return NodeType::FlavorMatch;
    else if (auto* nodeptr = mECS.try_get<DialogueNode>(entityID))
        return NodeType::Dialogue;
    else if (auto* nodeptr = mECS.try_get<DiceNode>(entityID))
        return NodeType::Dice;
    else if (auto* nodeptr = sQuestECS.try_get<AcceptQuestNode>(entityID))
        return NodeType::AcceptQuest;
    else if (auto* nodeptr = mECS.try_get<ReturnQuestNode>(entityID))
        return NodeType::ReturnQuest;
    else if (auto* nodeptr = mECS.try_get<ObjectiveNode>(entityID))
        return NodeType::Objective;
    return NodeType::None;
}

Character::Character(void)
{
    sHeaderBackground = Application_LoadTexture("Data/BlueprintBackground.png");
    
    auto entityID = mECS.create();
    mECS.emplace<Pin>(entityID, GetNextID(), "", PinKind::Output);
    const auto& node = mECS.emplace<Node>(entityID, GetNextID());
    ed::SetNodePosition(node.ID, { 50.0f, 50.0f });
    mID = sNextID++;
}

void Character::RenderCreatePanel(void) noexcept
{
    if (mCreateNewNode)
    {
        mNewLinkPin = nullptr;
        return;
    }

    if (ed::BeginCreate(ImColor{ 255, 255, 255, 255 }, 2.0f))
    {
        auto showLabel = [](const char* label, ImColor color)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
            auto size = ImGui::CalcTextSize(label);

            auto padding = ImGui::GetStyle().FramePadding;
            auto spacing = ImGui::GetStyle().ItemSpacing;

            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

            auto rectMin = ImGui::GetCursorScreenPos() - padding;
            auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

            auto drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
            ImGui::TextUnformatted(label);
        };

        ed::PinId startPinId = INVALID_PIN_ID, endPinId = INVALID_PIN_ID;
        if (ed::QueryNewLink(&startPinId, &endPinId))
        {
            auto startEntity = FindEntity(startPinId);
            auto endEntity = FindEntity(endPinId);
            auto& startPin = FindPin(startEntity, startPinId);
            auto& endPin = FindPin(endEntity, endPinId);

            //mNewLinkPin = startPinId != ed::PinId(0) ? startPin : endPin;
            if (startPin.Kind == PinKind::Input)
            {
                std::swap(startPin, endPin);
                std::swap(startPinId, endPinId);
            }

            if (startPinId != INVALID_PIN_ID && endPinId != INVALID_PIN_ID)
            {
                if (endPinId == startPinId)
                {
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                }
                else if (endPin.Kind == startPin.Kind)
                {
                    showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                }
                else if (startEntity == endEntity)
                {
                    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                }
                else if (endPin.Type != startPin.Type)
                {
                    showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                }
                else
                {
                    showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                    if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                    {
                        auto entityID = mECS.create();
                        auto& link = mECS.emplace<Link>(entityID, GetNextID());
                        link.StartPinID = startPinId;
                        link.EndPinID = endPinId;
                    }
                }
            }
        }

        ed::PinId pinId = INVALID_PIN_ID;
        if (ed::QueryNewNode(&pinId))
        {
            auto entityID = FindEntity(pinId);
            Pin* pin = nullptr;

            if (auto* output = mECS.try_get<Pin>(entityID))
                if (output->ID == pinId)
                    pin = output;
            if (auto* pins = mECS.try_get<InputOutput>(entityID))
            {
                if (pins->Input.ID == pinId)
                    pin = &pins->Input;
                else if (pins->Output.ID == pinId)
                    pin = &pins->Output;
            }
            else if (auto* pins = mECS.try_get<ForkInputOutput>(entityID))
            {
                if (pins->Input.ID == pinId)
                    pin = &pins->Input;
                else if (pins->Outputs[0].ID == pinId)
                    pin = &pins->Outputs[0];
                else if (pins->Outputs[1].ID == pinId)
                    pin = &pins->Outputs[1];
            }
            else if (auto* pins = mECS.try_get<InputOutputs>(entityID))
            {
                if (pins->Input.ID == pinId)
                    pin = &pins->Input;
                for (auto& output : pins->Outputs)
                    if (output.ID == pinId)
                        pin = &output;
            }
            mNewLinkPin = pin;
            if (mNewLinkPin)
                showLabel("+ Create Node", ImColor(32, 45, 32, 180));

            if (ed::AcceptNewItem())
            {
                mCreateNewNode = true;
                mNewNodeLinkPin = mNewLinkPin;
                mNewLinkPin = nullptr;
                ed::Suspend();
                ImGui::OpenPopup("Create New Node");
                ed::Resume();
            }
        }

    }
    else
        mNewLinkPin = nullptr;
    ed::EndCreate();

    if (ed::BeginDelete())
    {
        ed::LinkId linkId = 0;

        while (ed::QueryDeletedLink(&linkId))
        {
            if (ed::AcceptDeletedItem())
            {
                auto view = mECS.view<Link>();
                for (auto&& [entityID, link] : view.each())
                {
                    if (link.ID == linkId)
                    {
                        mECS.erase<Link>(entityID);
                        break;
                    }
                }
            }
        }

        ed::NodeId nodeId = 0;
        while (ed::QueryDeletedNode(&nodeId))
        {
            if (ed::AcceptDeletedItem())
            {
                auto deleteNode = [this, nodeId] (auto view) {
                    for (auto&& [entityID, node] : view.each())
                    {
                        if (node.ID == nodeId)
                        {
                            mECS.destroy(entityID);
                            break;
                        }
                    }
                };
                deleteNode(mECS.view<VariableNode<bool>>());
                deleteNode(mECS.view<VariableNode<int32_t>>());
                deleteNode(mECS.view<ActNode>());
                deleteNode(mECS.view<ForkNode>());
                deleteNode(mECS.view<BranchNode>());
                deleteNode(mECS.view<DialogueNode>());
                deleteNode(mECS.view<FlavorCheckNode>());
                deleteNode(mECS.view<FlavorMatchNode>());
                deleteNode(mECS.view<DiceNode>());
                deleteNode(mECS.view<CommentNode>());
                deleteNode(mECS.view<ReturnQuestNode>());
                deleteNode(mECS.view<ObjectiveNode>());
                auto view = sQuestECS.view<AcceptQuestNode>();
                for (auto&& [entityID, node] : view.each())
                {
                    if (node.ID == nodeId)
                    {
                        sQuestECS.destroy(entityID);
                        break;
                    }
                }
                
            }
        }
    }
    ed::EndDelete();
}

void Character::RenderLinks(void)
{
    auto view = mECS.view<Link>();
    view.each([](const auto entityID, auto& link) {
        ed::Link(link.ID, link.StartPinID, link.EndPinID, Link::COLOR, 2.0f); });
}

void Character::RenderFlow(void)
{
    auto view = mECS.view<Link>();
    for (auto& [entityID, link] : view.each())
        ed::Flow(link.ID);
}

[[nodiscard]] std::pair<Node*, NodeType> Character::FindNextNode(int32_t id, StateMachine& state, std::pair<Flavor, Flavor> flavors, int32_t choice)
{
    auto curr = FindEntity(Node{id}.ID);
    if (curr == entt::null)
        return { nullptr, NodeType::None };

    NodeType type = FindNodeType(curr);
    Node* node = FindNodes(AnyNode{}, curr);
    switch (type)
    {
    case NodeType::Entry:
    {
        const auto& output = mECS.get<Pin>(curr);
        auto* link = FindLink(output.ID);
        if (link == nullptr)
            return { nullptr, NodeType::None };
        auto next = FindEntity(link->EndPinID);
        return { FindNodes(AnyNode{}, next), FindNodeType(next) };
    }
    case NodeType::Fork:
    {
        const auto& pins = mECS.get<ForkInputOutput>(curr);
        const auto key = ((ForkNode*)node)->UUID.str();
        auto* link = FindLink(!state.Exists(key) ? pins.Outputs[1].ID : pins.Outputs[0].ID);
        state.SetValue(key, true);
        if (link == nullptr)
            return { nullptr, NodeType::None };
        auto next = FindEntity(link->EndPinID);
        return { FindNodes(AnyNode{}, next), FindNodeType(next) };
    }
    case NodeType::BoolVariable:
    case NodeType::IntVariable:
    {
        const auto& pins = mECS.get<InputOutput>(curr);
        
        if (type == NodeType::BoolVariable)
        {
            auto* varNode = (VariableNode<bool>*)node;
            state.SetValue(varNode->VariableName, varNode->Value);
        }
        else
        {
            auto* varNode = (VariableNode<int32_t>*)node;
            auto val = state.GetIntValue(varNode->VariableName);
            switch (varNode->Operator)
            {
            case SetOperator::Assignment:   val  = varNode->Value;  break;
            case SetOperator::Add:          val += varNode->Value;  break;
            case SetOperator::Subtract:     val -= varNode->Value;  break;
            case SetOperator::Multiple:     val *= varNode->Value;  break;
            case SetOperator::Divide:       val /= varNode->Value;  break;
            default:                                                break;
            }
            state.SetValue(varNode->VariableName, val);
        }

        auto* link = FindLink(pins.Output.ID);
        if (link == nullptr)
            return { nullptr, NodeType::None };
        auto next = FindEntity(link->EndPinID);
        return { FindNodes(AnyNode{}, next), FindNodeType(next) };
    }
    case NodeType::Branch:
    {
        const auto& pins = mECS.get<InputOutputs>(curr);
        auto* branchNode = (BranchNode*)node;
        int32_t i;
        for (i = 0; i < branchNode->Expressions.size(); i++)
        {
            auto& expression = branchNode->Expressions[i];
            bool expressionResult = true;
            for (const auto& condition : expression)
            {
                if (state.Exists(condition.VariableName))//Check for bool
                {
                    int32_t val = state.GetBoolValue(condition.VariableName);
                    if (val != condition.Value)//Don't need to switch on operator
                    {
                        expressionResult = false;
                        break;
                    }
                }
                else
                {
                    int32_t val = state.GetIntValue(condition.VariableName);
                    bool result;
                    switch (condition.Operator)
                    {
                    case CompareOperator::Equality:         result = val == condition.Value; break;
                    case CompareOperator::Different:        result = val != condition.Value; break;
                    case CompareOperator::Less:             result = val < condition.Value; break;
                    case CompareOperator::Greater:          result = val > condition.Value; break;
                    case CompareOperator::LessEquals:       result = val <= condition.Value; break;
                    case CompareOperator::GreaterEquals:    result = val >= condition.Value; break;
                    default:                                result = false; break;
                    }
                    if (!result)
                    {
                        expressionResult = false;
                        break;
                    }
                }
            }
            if (expressionResult)
                break;
        }
        auto* link = FindLink(pins.Outputs[i].ID);
        if (link == nullptr)
            return { nullptr, NodeType::None };
        auto next = FindEntity(link->EndPinID);
        return { FindNodes(AnyNode{}, next), FindNodeType(next) };
    }
    case NodeType::Dialogue:
    {
        const auto& pins = mECS.get<InputOutputs>(curr);
        auto* link = FindLink(pins.Outputs[choice].ID);
        if (link == nullptr)
            return { nullptr, NodeType::None };
        auto next = FindEntity(link->EndPinID);
        return { FindNodes(AnyNode{}, next), FindNodeType(next) };
    }
    case NodeType::Act:
    {
        const auto& pins = mECS.get<InputOutput>(curr);
        auto* link = FindLink(pins.Output.ID);
        if (link == nullptr)
            return { nullptr, NodeType::None };
        auto next = FindEntity(link->EndPinID);
        return { FindNodes(AnyNode{}, next), FindNodeType(next) };
    }
    case NodeType::FlavorMatch:
    {
        const auto& pins = mECS.get<ForkInputOutput>(curr);
        auto&& [mainFlavor, npcFlavor] = flavors;
        auto& pin = pins.Outputs[IsFlavorMatching(mainFlavor, npcFlavor) ? 0 : 1];
        auto* link = FindLink(pin.ID);
        if (link == nullptr)
            return { nullptr, NodeType::None };
        auto next = FindEntity(link->EndPinID);
        return { FindNodes(AnyNode{}, next), FindNodeType(next) };
    }
    case NodeType::FlavorCheck:
    {
        const auto& pins = mECS.get<InputOutputs>(curr);
        const auto& node = mECS.get<FlavorCheckNode>(curr);
        const auto [mainFlavor, npcFlavor] = flavors;
        const auto flavor = node.CheckingNPC ? npcFlavor : mainFlavor;
        const size_t index = static_cast<size_t>(flavor);
        if (index > 4)
            return { nullptr, NodeType::None };
        auto* link = FindLink(pins.Outputs[index].ID);
        if (link == nullptr)
            return { nullptr, NodeType::None };

        auto next = FindEntity(link->EndPinID);
        return { FindNodes(AnyNode{}, next), FindNodeType(next) };
    }
    case NodeType::Dice:
    {
        const auto& pins = mECS.get<InputOutputs>(curr);
        const float index = pins.Outputs.size() * Random::Float();
        auto* link = FindLink(pins.Outputs[static_cast<int32_t>(index)].ID);
        if (link == nullptr)
            return { nullptr, NodeType::None };
        auto next = FindEntity(link->EndPinID);
        return { FindNodes(AnyNode{}, next), FindNodeType(next) };
        break;
    }
    case NodeType::AcceptQuest:
    case NodeType::ReturnQuest:
    case NodeType::Objective:
    {
        const auto& pins = mECS.get<InputOutput>(curr);
        auto* link = FindLink(pins.Output.ID);
        if (link == nullptr)
            return { nullptr, NodeType::None };
        auto next = FindEntity(link->EndPinID);
        return { FindNodes(AnyNode{}, next), FindNodeType(next) };
    }
    default:
        break;
    }

    return { nullptr, NodeType::None };
}

static bool IsFlavorMatching(Flavor lhs, Flavor rhs)
{
    if (lhs == rhs)
        return true;
    else if (lhs == Flavor::Bitter &&
        (rhs == Flavor::BitterSalty || rhs == Flavor::BitterSweet || rhs == Flavor::BitterSour))
        return true;
    else if (lhs == Flavor::Salty &&
        (rhs == Flavor::SaltySour || rhs == Flavor::SaltySweet))
        return true;
    else if (rhs == Flavor::Bitter &&
        (lhs == Flavor::BitterSalty || lhs == Flavor::BitterSweet || lhs == Flavor::BitterSour))
        return true;
    else if (rhs == Flavor::Salty &&
        (lhs == Flavor::SaltySour || lhs == Flavor::SaltySweet))
        return true;
    else if (lhs == Flavor::Sour &&
        (rhs == Flavor::BitterSour || rhs == Flavor::SaltySour || rhs == Flavor::SweetSour))
        return true;
    else if (rhs == Flavor::Sour &&
        (lhs == Flavor::BitterSour || lhs == Flavor::SaltySour || lhs == Flavor::SweetSour))
        return true;
    else if ((lhs == Flavor::Sweet || lhs == Flavor::Sour) && rhs == Flavor::SweetSour)
        return true;
    else if ((rhs == Flavor::Sweet || rhs == Flavor::Sour) && lhs == Flavor::SweetSour)
        return true;
    else
        return false;
}

static bool IsFlavorSame(Flavor lhs, Flavor rhs) { return lhs == rhs; }

static void AddNewLines(char* text, size_t N)
{
    size_t length = strlen(text);
    if (length < N)
        return;
    int32_t numOfNewLines = (length - 1) / N;

    for (int32_t i = length - 1; i >= 0; --i)
    {
        if (i % N == 0 && i != 0 && text[i] != '\n')
        {
            memmove(text + i + 1, text + i, length);
            text[i] = '\n';
        }
    }
}
