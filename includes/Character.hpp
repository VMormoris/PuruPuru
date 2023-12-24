#pragma once

#include <Character.h>
#include <Components.h>
inline const ImVec2 OPERATOR_BUTTON_SIZE = { 32.0f, 28.0f };


//template<typename T>
//inline void Character::BuildNode(const T& node)
//{
//
//}
//
//template<typename T, typename ...Args>
//inline void Character::BuildNodes(void)
//{
//	auto view = mECS.view<T>();
//	for(auto& [entityID, node] : view.each())
//		BuildNode<T>(node);
//	BuildNodes<Args...>();
//}

template<typename T>
inline [[nodiscard]] entt::entity Character::SearchEntity(ed::NodeId nodeId) const
{
	auto view = mECS.view<T>();
	for (auto&& [entityID, node] : view.each())
		if (node.ID == nodeId)
			return entityID;
	return entt::null;
}

template<>
inline [[nodiscard]] entt::entity Character::SearchEntity<AcceptQuestNode>(ed::NodeId nodeId) const {
	auto view = sQuestECS.view<AcceptQuestNode>();
	for (auto&& [entityID, node] : view.each())
		if (node.ID == nodeId)
			return entityID;
	return entt::null;
}

template<typename T>
inline [[nodiscard]] Node* Character::FindNode(entt::entity entityID) const
{
	if (auto* node = mECS.try_get<T>(entityID))
		return (Node*)node;

	return nullptr;
}

template<>
inline [[nodiscard]] Node* Character::FindNode<AcceptQuestNode>(entt::entity entityID) const
{
	if (auto* node = mECS.try_get<AcceptQuestNode>(entityID))
		return (Node*)node;

	return nullptr;
}

template<typename T, typename ...Args>
inline [[nodiscard]] Node* Character::FindNodes(ComponentGroup<T, Args...>, entt::entity entityID) const
{
	if (Node* node = FindNode<T>(entityID))
		return node;

	if constexpr (sizeof...(Args) > 0)
		return FindNodes<Args...>(ComponentGroup<Args...>{}, entityID);
	return nullptr;
}

template<typename T>
inline void Character::RenderVariableNode(NodeBuilder& builder)
{
	static const char* operators[] = { "=", "+=", "-=", "*=", "/=" };
	auto view = mECS.view<VariableNode<T>>();

	for (auto&& [entityID, node] : view.each())
	{
		ImGui::PushID((int32_t)(int64_t)node.ID.AsPointer());
		builder.Begin(node.ID);
		RenderHeader(builder, VariableNode<T>::NAME, VariableNode<T>::COLOR);

		const auto& pins = mECS.get<InputOutput>(entityID);
		RenderInput(builder, pins.Input);
		builder.Middle();
		ImGui::Spring(1.0f, 0.0f);

		ImGui::PushItemWidth(128.0f);
		ImGui::InputText("##name", node.VariableName, STR_LENGTH);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		bool pressOperator = false;
		if constexpr (std::is_same<T, bool>::value)
		{
			ImGui::TextUnformatted("=");
			ImGui::SameLine();
			ImGui::Checkbox("##value", &node.Value);
		}
		else if constexpr (std::is_same<T, int32_t>::value)
		{
			const int32_t iOperators = static_cast<int32_t>(node.Operator);
			pressOperator = ImGui::Button(operators[iOperators], OPERATOR_BUTTON_SIZE);
			ImGui::SameLine();
			ImGui::PushItemWidth(64.0f);
			ImGui::DragScalar("##value", ImGuiDataType_S32, &node.Value, 1.0f);
			ImGui::PopItemWidth();
		}
		ImGui::Spring(0.0f, 1.0f);
		RenderOutput(builder, pins.Output);
		builder.End();

		ed::Suspend();
		if (pressOperator)
			ImGui::OpenPopup("Operator");

		if (ImGui::BeginPopup("Operator"))
		{
			for (int32_t i = 0; i < IM_ARRAYSIZE(operators); i++)
				if (ImGui::MenuItem(operators[i]))
					node.Operator = static_cast<SetOperator>(i);
			ImGui::EndPopup();
		}
		ed::Resume();
		ImGui::PopID();
	}
}

template<typename T>
inline [[nodiscard]] entt::entity Character::SpawnSetVariableNode()
{
	const auto entityID = mECS.create();
	auto& node = mECS.emplace<VariableNode<T>>(entityID, GetNextID());

	auto& pins = mECS.emplace<InputOutput>(entityID);
	pins.Input = { GetNextID(), "", PinType::Flow };
	pins.Input.Kind = PinKind::Input;

	pins.Output = { GetNextID(), "", PinType::Flow };
	pins.Output.Kind = PinKind::Output;

	return entityID;
}