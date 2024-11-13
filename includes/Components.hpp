#pragma once
#include <Components.h>
#include <yaml-cpp/yaml.h>

#include <stdint.h>

template<>
inline const char VariableNode<int32_t>::NAME[STR_LENGTH] = "Set Variable(Integer)";
template<>
inline const ImColor VariableNode<int32_t>::COLOR = { 128, 255, 64 };

inline const char DialogueNode::NAME[STR_LENGTH] = "DialogueNode";
inline const ImColor DialogueNode::COLOR = { 188, 43, 43 };

template<>
inline const char VariableNode<bool>::NAME[STR_LENGTH] = "Set Variable(Boolean)";
template<>
inline const ImColor VariableNode<bool>::COLOR = { 128, 255, 64 };

inline const char BranchNode::NAME[STR_LENGTH] = "Branch";
inline const ImColor BranchNode::COLOR = { 255, 128, 128 };

inline const char ForkNode::NAME[STR_LENGTH] = "Fork";
inline const ImColor ForkNode::COLOR = { 128, 195, 248 };

inline const char ActNode::NAME[STR_LENGTH] = "Act";
inline const ImColor ActNode::COLOR = { 128.0f, 128.0f, 128.0f };

inline const char FlavorMatchNode::NAME[STR_LENGTH] = "Flavor Match";
inline const ImColor FlavorMatchNode::COLOR = { 255, 166, 166 };

inline const char FlavorCheckNode::NAME[STR_LENGTH] = "Flavor Check";
inline const ImColor FlavorCheckNode::COLOR = { 255, 205, 205 };

inline const char DiceNode::NAME[STR_LENGTH] = "Dice";
inline const ImColor DiceNode::COLOR = { 255, 255, 0 };

inline const char AcceptQuestNode::NAME[STR_LENGTH] = "Accept Quest";
inline const ImColor AcceptQuestNode::COLOR = { 255, 0, 255 };

inline const char ReturnQuestNode::NAME[STR_LENGTH] = "Return Quest";
inline const ImColor ReturnQuestNode::COLOR = { 253, 96, 253 };

inline const char ObjectiveNode::NAME[STR_LENGTH] = "Objective";
inline const ImColor ObjectiveNode::COLOR = { 191, 0, 191 };

inline const ImColor Link::COLOR = { 255, 255, 255 };

namespace YAML {
	
	template<>
	struct convert<std::array<Pin, 2>> {

		static Node encode(const std::array<Pin, 2>& rhs)
		{
			Node node;
			node.push_back((uint64_t)rhs[0].ID.AsPointer());
			node.push_back((uint64_t)rhs[1].ID.AsPointer());
			node.SetStyle(EmitterStyle::Flow);
			return  node;
		}
	};

	template<>
	struct convert<std::vector<Pin>> {
		static Node encode(const std::vector<Pin>& rhs)
		{
			Node node;
			for(const auto& pin : rhs)
				node.push_back((int64_t)pin.ID.AsPointer());
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
	};

	template<>
	struct convert<ImVec2> {
		static Node encode(const ImVec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, ImVec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

}