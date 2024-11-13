#include <SceneSerializer.h>
#include <Components.h>
#include <Nodes.hpp>

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iostream>

static std::string SerializeSetOperator(SetOperator pOperator);
static std::string SerializeCompareOperator(CompareOperator pOperator);
static std::string SerializeSpeaker(Speaker pSpeaker);

static SetOperator DeserializeSetOperator(const std::string& pOperator);
static CompareOperator DeserializeCompareOperator(const std::string& pOperator);
static Speaker DeserializeSpeaker(const std::string& pSpeaker);

SceneSerializer::SceneSerializer(Scene* scene)
	: mScene(scene) {}

void SceneSerializer::Serialize(const std::string& filepath)
{
	YAML::Emitter out;
	out << YAML::BeginSeq;
	for (const auto& characterData : mScene->mAllData)
	{
		ed::SetCurrentEditor(characterData.Editor);
		out << YAML::BeginMap;
		const auto& character = characterData.Self;
		out << YAML::Key << "Name" << YAML::Value << characterData.Name;
		out << YAML::Key << "EntryNode" << YAML::Value;
		{
			auto view = character.mECS.view<Node, Pin>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pin] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "Output" << YAML::Value << (int64_t)pin.ID.AsPointer();
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
		out << YAML::Key << "VariableNodes" << YAML::Value;
		{
			auto view = character.mECS.view<VariableNode<bool>, InputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "Type" << YAML::Value << "Boolean";
				out << YAML::Key << "Name" << YAML::Value << node.VariableName;
				out << YAML::Key << "Operator" << YAML::Value << SerializeSetOperator(node.Operator);
				out << YAML::Key << "Value" << YAML::Value << node.Value;
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Output" << YAML::Value << (int64_t)pins.Output.ID.AsPointer();
				out << YAML::EndMap;
			}
		}
		{
			auto view = character.mECS.view<VariableNode<int32_t>, InputOutput>();
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "Type" << YAML::Value << "Integer";
				out << YAML::Key << "Name" << YAML::Value << node.VariableName;
				out << YAML::Key << "Operator" << YAML::Value << SerializeSetOperator(node.Operator);
				out << YAML::Key << "Value" << YAML::Value << node.Value;
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Output" << YAML::Value << (int64_t)pins.Output.ID.AsPointer();
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		out << YAML::Key << "ActNodes" << YAML::Value;
		{
			auto view = character.mECS.view<ActNode, InputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int32_t)(u64)node.ID.AsPointer();
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "Title" << YAML::Value << node.Title;
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Output" << YAML::Value << (int64_t)pins.Output.ID.AsPointer();

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
		out << YAML::Key << "BranchNodes" << YAML::Value;
		{
			auto view = character.mECS.view<BranchNode, InputOutputs>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Expressions" << YAML::Value;
				out << YAML::BeginSeq;
				for (const auto& expression : node.Expressions)
				{
					out << YAML::BeginSeq;
					for (const auto& condition : expression)
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
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Outputs" << YAML::Value << pins.Outputs;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
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
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Outputs" << YAML::Value << pins.Outputs;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		out << YAML::Key << "ForkNodes" << YAML::Value;
		{
			auto view = character.mECS.view<ForkNode, ForkInputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "UUID" << YAML::Value << node.UUID.str();
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Outputs" << YAML::Value << pins.Outputs;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		out << YAML::Key << "FlavorMatchNodes" << YAML::Value;
		{
			auto view = character.mECS.view<FlavorMatchNode, ForkInputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Outputs" << YAML::Value << pins.Outputs;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
		out << YAML::Key << "FlavorCheckNodes" << YAML::Value;
		{
			auto view = character.mECS.view<FlavorCheckNode, InputOutputs>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "ForNpc" << YAML::Value << node.CheckingNPC;
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Outputs" << YAML::Value << pins.Outputs;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
		out << YAML::Key << "DiceNodes" << YAML::Value;
		{
			auto view = character.mECS.view<DiceNode, InputOutputs>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Outputs" << YAML::Value << pins.Outputs;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
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
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
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
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Output" << YAML::Value << (int64_t)pins.Output.ID.AsPointer();
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
		out << YAML::Key << "ReturnQuestNodes" << YAML::Value;
		{
			auto view = character.mECS.view<ReturnQuestNode, InputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "QuestID" << YAML::Value << node.QuestID.str();
				out << YAML::Key << "Succeed" << YAML::Value << node.Succeed;
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Output" << YAML::Value << (int64_t)pins.Output.ID.AsPointer();
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
		out << YAML::Key << "ObjectiveNodes" << YAML::Value;
		{
			auto view = character.mECS.view<ObjectiveNode, InputOutput>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node, pins] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "QuestID" << YAML::Value << node.QuestID.str();
				out << YAML::Key << "ObjectiveID" << YAML::Value << node.ObjectiveID.str();
				out << YAML::Key << "Succeed" << YAML::Value << node.Succeed;
				out << YAML::Key << "Input" << YAML::Value << (int64_t)pins.Input.ID.AsPointer();
				out << YAML::Key << "Output" << YAML::Value << (int64_t)pins.Output.ID.AsPointer();
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
		out << YAML::Key << "Comments" << YAML::Value;
		{
			auto view = character.mECS.view<CommentNode>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, node] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int64_t)node.ID.AsPointer();
				out << YAML::Key << "Comment" << YAML::Value << node.Comment;
				out << YAML::Key << "Position" << YAML::Value << ed::GetNodePosition(node.ID);
				out << YAML::Key << "Size" << YAML::Value << ed::GetNodeSize(node.ID);
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
		out << YAML::Key << "Links" << YAML::Value;
		{
			auto view = character.mECS.view<Link>();
			out << YAML::BeginSeq;
			for (auto&& [entityID, link] : view.each())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "ID" << YAML::Value << (int32_t)(u64)link.ID.AsPointer();
				out << YAML::Key << "StartPinID" << YAML::Value << (int32_t)(u64)link.StartPinID.AsPointer();
				out << YAML::Key << "EndPinID" << YAML::Value << (int32_t)(u64)link.EndPinID.AsPointer();
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
		out << YAML::EndMap;
	}
	out << YAML::EndSeq;
	std::ofstream os(filepath, std::ios::binary);
	os.write(out.c_str(), out.size());
	os.close();
}

void SceneSerializer::Deserialize(const std::string& filepath)
{
	mScene->mAllData.clear();
	Character::sQuestECS.clear();
	Character::sNextID = 0;

	std::ifstream is(filepath);
	YAML::Node data;
	try { data = YAML::Load(is); }
	catch (YAML::ParserException e) { std::cout << e.msg << '\n';  return; }

	for (auto characterNode : data)
	{
		int32_t nextID = 1;
		ed::Config config;
		auto* editor = ed::CreateEditor(&config);
		ed::SetCurrentEditor(editor);
		Scene::CharacterData characterData{editor};
		auto& character = characterData.Self;

		std::string name = characterNode["Name"].as<std::string>();
		memcpy(characterData.Name, name.c_str(), name.size() + 1);

		if (const auto& nodes = characterNode["EntryNode"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();
				const int32_t pinId = node["Output"].as<int32_t>();

				for (auto entityID : character.mECS.view<Node>())
					character.mECS.destroy(entityID);

				auto entity = character.mECS.create();
				auto& newNode = character.mECS.emplace<Node>(entity, id);
				ed::SetNodePosition(newNode.ID, pos);
				character.mECS.emplace<Pin>(entity, pinId, "", PinKind::Output);
			}
		}

		if (const auto& nodes = characterNode["VariableNodes"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();
				const std::string type = node["Type"].as<std::string>();

				auto entity = character.mECS.create();
				if (type.compare("Boolean") == 0)
				{
					auto& variable = character.mECS.emplace<VariableNode<bool>>(entity, id);
					auto name = node["Name"].as<std::string>();
					memcpy(variable.VariableName, name.c_str(), name.size() + 1);
					variable.Operator = DeserializeSetOperator(node["Operator"].as<std::string>());
					variable.Value = node["Value"].as<bool>();
					ed::SetNodePosition(variable.ID, pos);
				}
				else if (type.compare("Integer") == 0)
				{
					auto& variable = character.mECS.emplace<VariableNode<int32_t>>(entity, id);
					auto name = node["Name"].as<std::string>();
					memcpy(variable.VariableName, name.c_str(), name.size() + 1);
					variable.Operator = DeserializeSetOperator(node["Operator"].as<std::string>());
					variable.Value = node["Value"].as<int32_t>();
					ed::SetNodePosition(variable.ID, pos);
				}

				auto& pins = character.mECS.emplace<InputOutput>(entity);
				const int32_t inputID = node["Input"].as<int32_t>();
				const int32_t outputID = node["Output"].as<int32_t>();

				pins.Input = Pin{ inputID, PinKind::Input };
				pins.Output = Pin{ outputID, PinKind::Output };
				nextID = std::max({ inputID, outputID, id, nextID });
			}
		}

		if (const auto& nodes = characterNode["ActNodes"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();
				auto entity = character.mECS.create();
				auto& act = character.mECS.emplace<ActNode>(entity, id);
				auto title = node["Title"].as<std::string>();
				memcpy(act.Title, title.c_str(), title.size() + 1);
				
				const auto& bubbles = node["Bubbles"];
				for (const auto& bubble : bubbles)
					act.Bubbles.emplace_back(std::make_pair(
						DeserializeSpeaker(bubble["Speaker"].as<std::string>()),
						bubble["Line"].as<std::string>()
					));
				
				auto& pins = character.mECS.emplace<InputOutput>(entity);
				const int32_t inputID = node["Input"].as<int32_t>();
				const int32_t outputID = node["Output"].as<int32_t>();
				pins.Input = Pin{ inputID, PinKind::Input };
				pins.Output = Pin{ outputID, PinKind::Output };
				ed::SetNodePosition(act.ID, pos);
				nextID = std::max({ inputID, outputID, id, nextID });
			}
		}

		if (const auto& nodes = characterNode["ForkNodes"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				nextID = std::max(id, nextID);
				const auto pos = node["Position"].as<ImVec2>();

				const gte::uuid uuid = node["UUID"].as<std::string>();
				auto entity = character.mECS.create();
				auto& fork = character.mECS.emplace<ForkNode>(entity, id, uuid);

				auto& pins = character.mECS.emplace<ForkInputOutput>(entity);
				const int32_t inputID = node["Input"].as<int32_t>();
				const int32_t outputID0 = node["Outputs"][0].as<int32_t>();
				const int32_t outputID1 = node["Outputs"][1].as<int32_t>();

				pins.Input = Pin{ inputID, PinKind::Input };
				pins.Outputs[0] = Pin{ outputID0, "Others", PinKind::Output };
				pins.Outputs[1] = Pin{ outputID1, "First", PinKind::Output };
				ed::SetNodePosition(fork.ID, pos);
				nextID = std::max({ inputID, outputID0, outputID1, id, nextID });
			}
		}

		if (const auto& nodes = characterNode["BranchNodes"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();

				auto entity = character.mECS.create();
				auto& branch = character.mECS.emplace<BranchNode>(entity, id);

				if (const auto& expressions = node["Expressions"])
				{
					for (const auto& expr : expressions)
					{
						auto& expression = branch.Expressions.emplace_back();
						
						for (const auto& condition : expr)
						{
							auto& cond = expression.emplace_back();
							const std::string name = condition["Name"].as<std::string>();
							memcpy(cond.VariableName, name.c_str(), name.size() + 1);
							cond.Operator = DeserializeCompareOperator(condition["Operator"].as<std::string>());
							cond.Value = condition["Value"].as<int32_t>();
						}
					}
				}
				auto& pins = character.mECS.emplace<InputOutputs>(entity);
				const int32_t inputID = node["Input"].as<int32_t>();
				pins.Input = Pin{ inputID, PinKind::Input };

				const auto& outputs = node["Outputs"];
				int32_t maxOutputID = 1;
				for (int i = 0; i < outputs.size(); i++)
				{
					const auto& output = outputs[i];
					const int32_t outputID = output.as<int32_t>();
					pins.Outputs.emplace_back(outputID, "then", PinKind::Output);
					maxOutputID = std::max(maxOutputID, outputID);
				}
				pins.Outputs.back().Name = "else";
				ed::SetNodePosition(branch.ID, pos);
				nextID = std::max({ inputID, maxOutputID, id, nextID });
			}
		}

		if (const auto& nodes = characterNode["DialogueNodes"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();

				auto entity = character.mECS.create();
				auto& dialogue = character.mECS.emplace<DialogueNode>(entity, id);

				if (const auto& prompts = node["Prompts"])
				{
					for (const auto& prompt : prompts)
						dialogue.Prompts.emplace_back(prompt.as<std::string>());
				}

				auto& pins = character.mECS.emplace<InputOutputs>(entity);
				const int32_t inputID = node["Input"].as<int32_t>();
				pins.Input = Pin{ inputID, PinKind::Input };

				const auto& outputs = node["Outputs"];
				int32_t maxOutputID = 1;
				for (int i = 0; i < outputs.size(); i++)
				{
					const auto& output = outputs[i];
					const int32_t outputID = output.as<int32_t>();
					pins.Outputs.emplace_back(outputID, "", PinKind::Output);
					maxOutputID = std::max(maxOutputID, outputID);
				}
				ed::SetNodePosition(dialogue.ID, pos);
				nextID = std::max({ inputID, maxOutputID, id, nextID });
			}
		}

		if (const auto& nodes = characterNode["FlavorMatchNodes"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();

				auto entity = character.mECS.create();
				auto& flavorMatch = character.mECS.emplace<FlavorMatchNode>(entity, id);

				auto& pins = character.mECS.emplace<ForkInputOutput>(entity);
				const int32_t inputID = node["Input"].as<int32_t>();
				pins.Input = Pin{ inputID, PinKind::Input };

				const int32_t outputID0 = node["Outputs"][0].as<int32_t>();
				const int32_t outputID1 = node["Outputs"][1].as<int32_t>();

				pins.Input = Pin{ inputID, PinKind::Input };
				pins.Outputs[0] = Pin{ outputID0, "Flavor matching", PinKind::Output };
				pins.Outputs[1] = Pin{ outputID1, "else", PinKind::Output };
				ed::SetNodePosition(flavorMatch.ID, pos);
				nextID = std::max({ inputID, outputID0, outputID1, id, nextID });
			}
		}

		if (const auto& nodes = characterNode["FlavorCheckNodes"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();
				const bool forNpc = node["ForNpc"].as<bool>();
				auto entity = character.mECS.create();
				auto& flavorMatch = character.mECS.emplace<FlavorCheckNode>(entity, id, forNpc);

				auto& pins = character.mECS.emplace<InputOutputs>(entity);
				const int32_t inputID = node["Input"].as<int32_t>();
				pins.Input = Pin{ inputID, PinKind::Input };

				const int32_t outputID0 = node["Outputs"][0].as<int32_t>();
				const int32_t outputID1 = node["Outputs"][1].as<int32_t>();
				const int32_t outputID2 = node["Outputs"][2].as<int32_t>();
				const int32_t outputID3 = node["Outputs"][3].as<int32_t>();
				const int32_t outputID4 = node["Outputs"][4].as<int32_t>();

				pins.Outputs.emplace_back(outputID0, "Bitter", PinKind::Output);
				pins.Outputs.emplace_back(outputID1, "Salty", PinKind::Output);
				pins.Outputs.emplace_back(outputID2, "Sour", PinKind::Output);
				pins.Outputs.emplace_back(outputID3, "Sweet", PinKind::Output);
				pins.Outputs.emplace_back(outputID4, "Neutral", PinKind::Output);
				ed::SetNodePosition(flavorMatch.ID, pos);
				nextID = std::max({ inputID, outputID0, outputID1, outputID2, outputID3, outputID4, id, nextID });
			}
		}

		if (const auto& nodes = characterNode["DiceNodes"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();
				auto entity = character.mECS.create();
				auto& dice = character.mECS.emplace<DiceNode>(entity, id);

				auto& pins = character.mECS.emplace<InputOutputs>(entity);
				const int32_t inputID = node["Input"].as<int32_t>();
				pins.Input = Pin{ inputID, PinKind::Input };
				const auto& outputs = node["Outputs"];
				int32_t max = -1;
				for (const auto& output : outputs)
				{
					int32_t id = output.as<int32_t>();
					max = std::max(max, id);
					pins.Outputs.emplace_back(id, PinKind::Output);
				}

				ed::SetNodePosition(dice.ID, pos);
				nextID = std::max({ max, nextID, id, inputID });
			}
		}

		if (const auto& nodes = characterNode["AcceptQuestNodes"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();
				const gte::uuid uuid = node["UUID"].as<std::string>();
				const std::string title = node["Title"].as<std::string>();
				const std::string description = node["Description"].as<std::string>();
				auto entity = Character::sQuestECS.create();
				auto& quest = Character::sQuestECS.emplace<AcceptQuestNode>(entity, id);
				quest.UUID = uuid;
				quest.Owner = character.mID;
				strcpy(quest.Title, title.c_str());
				strcpy(quest.Description, description.c_str());
				if (const auto& objectives = node["Objectives"])
				{
					for (const auto& objective : objectives)
					{
						const gte::uuid uuid = objective["UUID"].as<std::string>();
						const std::string title = objective["Title"].as<std::string>();
						const std::string description = objective["Description"].as<std::string>();
						const bool isOptional = objective["IsOptional"].as<bool>();
						auto& obj = quest.Objectives.emplace_back();
						obj.UUID = uuid;
						strcpy(obj.Title, title.c_str());
						strcpy(obj.Description, description.c_str());
						obj.IsOptional = isOptional;
					}
				}
				auto& pins = Character::sQuestECS.emplace<InputOutput>(entity);
				const int32_t inputID = node["Input"].as<int32_t>();
				const int32_t outputID = node["Output"].as<int32_t>();
				pins.Input = Pin{ inputID, PinKind::Input };
				pins.Output = Pin{ outputID, PinKind::Output };
				ed::SetNodePosition(quest.ID, pos);
				nextID = std::max({ inputID, outputID, id, nextID });
			}
		}

		if (const auto& nodes = characterNode["ReturnQuestNodes"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();
				const gte::uuid questID = node["QuestID"].as<std::string>();
				bool succeed = true;
				if (node["Succeed"]) succeed = node["Succeed"].as<bool>();

				auto entity = character.mECS.create();
				auto& quest = character.mECS.emplace<ReturnQuestNode>(entity, id);
				quest.QuestID = questID;
				quest.Succeed = succeed;

				auto& pins = character.mECS.emplace<InputOutput>(entity);
				const int32_t inputID = node["Input"].as<int32_t>();
				const int32_t outputID = node["Output"].as<int32_t>();
				pins.Input = Pin{ inputID, PinKind::Input };
				pins.Output = Pin{ outputID, PinKind::Output };
				ed::SetNodePosition(quest.ID, pos);
				nextID = std::max({ inputID, outputID, id, nextID });
			}
		}

		if (const auto& nodes = characterNode["ObjectiveNodes"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();
				const gte::uuid questID = node["QuestID"].as<std::string>();
				const gte::uuid objectiveID = node["ObjectiveID"].as<std::string>();
				bool succeed = true;
				if (node["Succeed"]) succeed = node["Succeed"].as<bool>();

				auto entity = character.mECS.create();
				auto& objective = character.mECS.emplace<ObjectiveNode>(entity, id);
				objective.QuestID = questID;
				objective.ObjectiveID = objectiveID;
				objective.Succeed = succeed;

				auto& pins = character.mECS.emplace<InputOutput>(entity);
				const int32_t inputID = node["Input"].as<int32_t>();
				const int32_t outputID = node["Output"].as<int32_t>();
				pins.Input = Pin{ inputID, PinKind::Input };
				pins.Output = Pin{ outputID, PinKind::Output };
				ed::SetNodePosition(objective.ID, pos);
				nextID = std::max({ inputID, outputID, id, nextID });
			}
		}

		if (const auto& nodes = characterNode["Comments"])
		{
			for (const auto& node : nodes)
			{
				const int32_t id = node["ID"].as<int32_t>();
				const auto pos = node["Position"].as<ImVec2>();
				const auto size = node["Size"].as<ImVec2>();
				const auto comm = node["Comment"].as<std::string>();

				auto entity = character.mECS.create();
				auto& comment = character.mECS.emplace<CommentNode>(entity, id);
				comment.Comment = comm;
				comment.Size = size;
				ed::SetNodePosition(comment.ID, pos);
				nextID = std::max({ nextID, id });
			}
		}

		if (const auto& links = characterNode["Links"])
		{
			for (const auto& link : links)
			{
				const int32_t id = link["ID"].as<int32_t>();
				nextID = std::max(id, nextID);
				const int32_t start = link["StartPinID"].as<int32_t>();
				const int32_t end = link["EndPinID"].as<int32_t>();
				auto entity = character.mECS.create();

				character.mECS.emplace<Link>(entity, id, start, end);
			}
		}
		character.ResetID(nextID + 1);
		mScene->mAllData.emplace_back(std::move(characterData));
	}
	mScene->mWorkingDataIndex = 0;
}

std::string SerializeSetOperator(SetOperator pOperator)
{
	switch (pOperator)
	{
	case SetOperator::Assignment:	return "=";
	case SetOperator::Add:			return "+=";
	case SetOperator::Subtract:		return "-=";
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
	else//Should not reach here
		return Speaker::Internal;
}