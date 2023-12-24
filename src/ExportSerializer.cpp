#include <ExportSerializer.h>
#include <Components.h>
#include <Nodes.hpp>

#include <yaml-cpp/yaml.h>
#include <fstream>

static std::string SerializeSetOperator(SetOperator pOperator);
static std::string SerializeCompareOperator(CompareOperator pOperator);
static std::string SerializeSpeaker(Speaker pSpeaker);

static SetOperator DeserializeSetOperator(const std::string& pOperator);
static CompareOperator DeserializeCompareOperator(const std::string& pOperator);
static Speaker DeserializeSpeaker(const std::string& pSpeaker);

ExportSerializer::ExportSerializer(Scene* scene)
	: mScene(scene) {}

void ExportSerializer::Serialize(const std::string& filepath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Characters" << YAML::Value;
	out << YAML::BeginSeq;
	for (const auto& characterData : mScene->mAllData)
	{
		out << YAML::BeginMap;
		const auto& character = characterData.Self;

		// ---------------------------------------------------------------------------------
		// -------------------------------- Entry Node -------------------------------------
		// ---------------------------------------------------------------------------------
		
		out << YAML::Key << "Name" << YAML::Value << characterData.Name;
		out << YAML::Key << "EntryNode" << YAML::Value;
		{
			auto view = character.mECS.view<Node, Pin>();
			auto links = character.mECS.view<Link>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pin] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				auto targets = FindTargets(character.mECS, pin);
				out << YAML::Key << "Outputs" << YAML::Value << targets;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// ---------------------------------------------------------------------------------
		// -------------------------------- Variables Nodes --------------------------------
		// ---------------------------------------------------------------------------------

		out << YAML::Key << "VariableNodes" << YAML::Value;
		{
			auto view = character.mECS.view<VariableNode<bool>, InputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Type" << YAML::Value << "Boolean";
				out << YAML::Key << "Name" << YAML::Value << node.VariableName;
				out << YAML::Key << "Operator" << YAML::Value << SerializeSetOperator(node.Operator);
				out << YAML::Key << "Value" << YAML::Value << node.Value;
				auto targets = FindTargets(character.mECS, pins.Output);
				out << YAML::Key << "Outputs" << YAML::Value << targets;
				out << YAML::EndMap;
			}
		}
		{
			auto view = character.mECS.view<VariableNode<int32_t>, InputOutput>();
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Type" << YAML::Value << "Integer";
				out << YAML::Key << "Name" << YAML::Value << node.VariableName;
				out << YAML::Key << "Operator" << YAML::Value << SerializeSetOperator(node.Operator);
				out << YAML::Key << "Value" << YAML::Value << node.Value;
				auto targets = FindTargets(character.mECS, pins.Output);
				out << YAML::Key << "Outputs" << YAML::Value << targets;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// ---------------------------------------------------------------------------------
		// -------------------------------- Act Nodes --------------------------------------
		// ---------------------------------------------------------------------------------

		out << YAML::Key << "ActNodes" << YAML::Value;
		{
			auto view = character.mECS.view<ActNode, InputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int32_t)node.ID.AsPointer();
				out << YAML::Key << "Title" << YAML::Value << node.Title;
				auto targets = FindTargets(character.mECS, pins.Output);
				out << YAML::Key << "Outputs" << YAML::Value << targets;
				out << YAML::Key << "Bubbles" << YAML::Value;
				out << YAML::BeginSeq;
				for (auto&& [speaker, line] : node.Bubbles)
				{
					out << YAML::BeginMap;
					out << YAML::Key << "Speaker" << YAML::Value << SerializeSpeaker(speaker);
					out << YAML::Key << "Line" << YAML::Value << line;
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// ---------------------------------------------------------------------------------
		// -------------------------------- Fork Nodes -------------------------------------
		// ---------------------------------------------------------------------------------
		
		out << YAML::Key << "ForkNodes" << YAML::Value;
		{
			auto view = character.mECS.view<ForkNode, ForkInputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "UUID" << YAML::Value << node.UUID.str();
				
				out << YAML::Key << "Outputs" << YAML::Value;
				out << YAML::BeginSeq;
				for (const auto& output : pins.Outputs)
				{
					auto targets = FindTargets(character.mECS, output);
					out << targets;
				}
				out << YAML::EndSeq;
				
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// ---------------------------------------------------------------------------------
		// -------------------------------- Branch Nodes -----------------------------------
		// ---------------------------------------------------------------------------------

		out << YAML::Key << "BranchNodes" << YAML::Value;
		{
			auto view = character.mECS.view<BranchNode, InputOutputs>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Expressions" << YAML::Value;//a == true && b == true && ...
				out << YAML::BeginSeq;
				for (const auto& expressions : node.Expressions)
				{
					out << YAML::BeginSeq << YAML::Indent(1);
					for (const auto& condition : expressions)
					{
						out << YAML::BeginMap;
						out << YAML::Key << "Name" << YAML::Value << condition.VariableName;
						out << YAML::Key << "Operator" << YAML::Value << SerializeCompareOperator(condition.Operator);
						out << YAML::Key << "Value" << YAML::Value << condition.Value;
						out << YAML::EndMap;
					}
					out << YAML::EndSeq;
				}
				out << YAML::EndSeq;
				
				out << YAML::Key << "Outputs" << YAML::Value;
				out << YAML::BeginSeq;
				for (const auto& output : pins.Outputs)
				{
					auto targets = FindTargets(character.mECS, output);
					out << targets;
				}
				out << YAML::EndSeq;

				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// ---------------------------------------------------------------------------------
		// -------------------------------- Flavor Check Nodes -----------------------------
		// ---------------------------------------------------------------------------------

		out << YAML::Key << "FlavorCheckNodes" << YAML::Value;
		{
			auto view = character.mECS.view<FlavorCheckNode, InputOutputs>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "ForNpc" << YAML::Value << node.CheckingNPC;
				
				out << YAML::Key << "Outputs" << YAML::Value;
				out << YAML::BeginSeq;
				for (const auto& output : pins.Outputs)
				{
					auto targets = FindTargets(character.mECS, output);
					out << targets;
				}
				out << YAML::EndSeq;

				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// ---------------------------------------------------------------------------------
		// -------------------------------- Flavor Match Nodes -----------------------------
		// ---------------------------------------------------------------------------------
		
		out << YAML::Key << "FlavorMatchNodes" << YAML::Value;
		{
			auto view = character.mECS.view<FlavorMatchNode, ForkInputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				
				out << YAML::Key << "Outputs" << YAML::Value;
				out << YAML::BeginSeq;
				for (const auto& output : pins.Outputs)
				{
					auto targets = FindTargets(character.mECS, output);
					out << targets;
				}
				out << YAML::EndSeq;

				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// ---------------------------------------------------------------------------------
		// -------------------------------- Dialogue Nodes ---------------------------------
		// ---------------------------------------------------------------------------------
		out << YAML::Key << "DialogueNodes" << YAML::Value;
		{
			auto view = character.mECS.view<DialogueNode, InputOutputs>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Prompts" << YAML::Value;
				out << YAML::BeginSeq;
				for (const auto& prompt : node.Prompts)
					out << prompt;
				out << YAML::EndSeq;
				
				out << YAML::Key << "Outputs" << YAML::Value;
				out << YAML::BeginSeq;
				for (const auto& output : pins.Outputs)
				{
					auto targets = FindTargets(character.mECS, output);
					out << targets;
				}
				out << YAML::EndSeq;

				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// ---------------------------------------------------------------------------------
		// -------------------------------- Accept Quest Nodes -----------------------------
		// ---------------------------------------------------------------------------------
		out << YAML::Key << "AcceptQuestNodes" << YAML::Value;
		{
			auto view = Character::sQuestECS.view<AcceptQuestNode, InputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				if (node.Owner != character.mID)
					continue;
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "UUID" << YAML::Value << node.UUID.str();
				out << YAML::Key << "Title" << YAML::Value << node.Title;
				out << YAML::Key << "Description" << YAML::Value << node.Description;
				out << YAML::Key << "Objectives" << YAML::Value;
				out << YAML::BeginSeq;
				for (const auto& objective : node.Objectives)
				{
					out << YAML::BeginMap;
					out << YAML::Key << "UUID" << YAML::Value << objective.UUID.str();
					out << YAML::Key << "Title" << YAML::Value << objective.Title;
					out << YAML::Key << "Description" << YAML::Value << objective.Description;
					out << YAML::Key << "IsOptional" << YAML::Value << objective.IsOptional;
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
				out << YAML::Key << "Outputs" << YAML::Value << FindTargets(character.mECS, pins.Output);
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// ---------------------------------------------------------------------------------
		// -------------------------------- Return Quest Nodes -----------------------------
		// ---------------------------------------------------------------------------------
		out << YAML::Key << "ReturnQuestNodes" << YAML::Value;
		{
			auto view = character.mECS.view<ReturnQuestNode, InputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "QuestID" << YAML::Value << node.QuestID.str();
				out << YAML::Key << "Succeed" << YAML::Value << node.Succeed;
				out << YAML::Key << "Outputs" << YAML::Value << FindTargets(character.mECS, pins.Output);
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// ---------------------------------------------------------------------------------
		// -------------------------------- Objective Nodes --------------------------------
		// ---------------------------------------------------------------------------------
		out << YAML::Key << "ObjectiveNodes" << YAML::Value;
		{
			auto view = character.mECS.view<ObjectiveNode, InputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "QuestID" << YAML::Value << node.QuestID.str();
				out << YAML::Key << "ObjectiveID" << YAML::Value << node.ObjectiveID.str();
				out << YAML::Key << "Succeed" << YAML::Value << node.Succeed;
				out << YAML::Key << "Outputs" << YAML::Value << FindTargets(character.mECS, pins.Output);
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// ---------------------------------------------------------------------------------
		// -------------------------------- Dice Nodes -------------------------------------
		// ---------------------------------------------------------------------------------
		out << YAML::Key << "DiceNodes" << YAML::Value;
		{
			auto view = character.mECS.view<DiceNode, InputOutputs>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Outputs" << YAML::Value;
				out << YAML::BeginSeq;
				for (const auto& output : pins.Outputs)
				{
					auto targets = FindTargets(character.mECS, output);
					out << targets;
				}
				out << YAML::EndSeq;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		out << YAML::EndMap;
	}
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream ofs(filepath);
	ofs << out.c_str();
}


[[nodiscard]] Node* ExportSerializer::FindNode(const entt::registry& reg, ed::PinId pinId)
{
	{
		auto view = reg.view<InputOutput>();
		for (auto&& [entityID, node] : view.each())
			if (node.Input.ID == pinId)
				return FindNodes(AnyNode{}, reg, entityID);
	}


	{
		auto view = reg.view<ForkInputOutput>();
		for (auto&& [entityID, node] : view.each())
			if (node.Input.ID == pinId)
				return FindNodes(AnyNode{}, reg, entityID);
	}


	{
		auto view = reg.view<InputOutputs>();
		for (auto&& [entityID, node] : view.each())
			if (node.Input.ID == pinId)
				return FindNodes(AnyNode{}, reg, entityID);
	}

	return nullptr;
}

void ExportSerializer::SerializeLines(const std::string& filepath)
{
	std::ofstream ofs(filepath);
	for (const auto& characterData : mScene->mAllData)
	{
		auto view = characterData.Self.mECS.view<ActNode>();
		for (auto&& [entityID, act] : view.each())
			for (const auto& bubble : act.Bubbles)
				ofs << bubble.second << '\n';
	}
	ofs.close();
}



std::string SerializeSetOperator(SetOperator pOperator)
{
	switch (pOperator)
	{
	case SetOperator::Assignment:	return "=";
	case SetOperator::Add:			return "+=";
	case SetOperator::Subtract:	return "-=";
	case SetOperator::Multiple:		return "*=";
	case SetOperator::Divide:		return "/=";
	default:						return "";
	}
}

SetOperator DeserializeSetOperator(const std::string& pOperator)
{
	if (pOperator.compare("=") == 0)		return SetOperator::Assignment;
	else if (pOperator.compare("+=") == 0)	return SetOperator::Add;
	else if (pOperator.compare("-=") == 0)	return SetOperator::Subtract;
	else if (pOperator.compare("*=") == 0)	return SetOperator::Multiple;
	else if (pOperator.compare("/=") == 0)	return SetOperator::Divide;
	return SetOperator::Assignment;//Shouldn't be reached
}

std::string SerializeCompareOperator(CompareOperator pOperator)
{
	switch (pOperator)
	{
	case CompareOperator::Equality:			return "==";
	case CompareOperator::Greater:			return ">";
	case CompareOperator::Less:				return "<";
	case CompareOperator::GreaterEquals:	return ">=";
	case CompareOperator::LessEquals:		return "<=";
	case CompareOperator::Different:		return "<>";
	default:								return "";
	}
}

CompareOperator DeserializeCompareOperator(const std::string& pOperator)
{

	if (pOperator.compare("==") == 0)		return CompareOperator::Equality;
	else if (pOperator.compare(">") == 0)	return CompareOperator::Greater;
	else if (pOperator.compare("<") == 0)	return CompareOperator::Less;
	else if (pOperator.compare(">=") == 0)	return CompareOperator::GreaterEquals;
	else if (pOperator.compare("<=") == 0)	return CompareOperator::LessEquals;
	else if (pOperator.compare("<>") == 0)	return CompareOperator::Different;
	return CompareOperator::Equality;//Shouldn't be reached
}

std::string SerializeSpeaker(Speaker pSpeaker)
{
	switch (pSpeaker)
	{
	case Speaker::MainCharacter:	return "MainCharacter";
	case Speaker::NPC:				return "NPC";
	case Speaker::Internal:			return "Internal";
	default:						return "";
	}
}

Speaker DeserializeSpeaker(const std::string& pSpeaker)
{
	if (pSpeaker.compare("MainCharacter") == 0)	return Speaker::MainCharacter;
	else if (pSpeaker.compare("NPC") == 0)		return Speaker::NPC;
	else if (pSpeaker.compare("Internal") == 0) return Speaker::Internal;
}