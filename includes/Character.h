#pragma once
#include <imgui.h>

#include <vector>

#include "Components.h"
#include "StateMachine.h"

namespace util = ax::NodeEditor::Utilities;
namespace ed = ax::NodeEditor;

struct NodeIdLess
{
	bool operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const
	{
		return lhs.AsPointer() < rhs.AsPointer();
	}
};

//Forward declarations
struct Node;
struct Pin;
struct Link;

class Character {
	using NodeBuilder = util::BlueprintNodeBuilder;
public:
	Character();

	void UpdateTouch(void);

	void RenderNodes(void);
	void RenderLinks(void);
	void RenderCreatePanel(void) noexcept;
	void RenderFlow(void);

	[[nodiscard]] entt::entity SpawnBranchNode(void);
	[[nodiscard]] entt::entity SpawnForkNode(void);
	[[nodiscard]] entt::entity SpawnActNode(void);
	template<typename T>
	[[nodiscard]] entt::entity SpawnSetVariableNode(void);
	[[nodiscard]] entt::entity SpawnDialogueNode(void);

	//
	[[nodiscard]] entt::entity SpawnFlavorMatch(void);
	[[nodiscard]] entt::entity SpawnFlavorCheck(bool forNpc);
	[[nodiscard]] entt::entity SpawnDiceNode(void);
	[[nodiscard]] entt::entity SpawnAcceptQuestNode(void);
	[[nodiscard]] entt::entity SpawnReturnQuestNode(void);
	[[nodiscard]] entt::entity SpawnObjectiveNode(void);
	entt::entity SpawnCommentNode(void);

	[[nodiscard]] entt::entity FindEntity(ed::NodeId nodeId) const;
	[[nodiscard]] std::pair<Node*, NodeType> FindNextNode(int32_t id, StateMachine& state, std::pair<Flavor, Flavor> flavors, int32_t choice = -1);

	template<typename T, typename ...Args>
	[[nodiscard]] Node* FindNodes(ComponentGroup<T, Args...>, entt::entity entityID) const;
	[[nodiscard]] NodeType FindNodeType(entt::entity entityID);
	[[nodiscard]] int32_t GetEntryNodeID(void);

	bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);
	
	void HandleInput(void);

	void SetupVariables(StateMachine& stateMachine) const;

private:

	[[nodiscard]] std::string FindSelectedQuestTitle(const gte::uuid& selection);
	[[nodiscard]] std::string FindSelectedObjectiveTitle(const gte::uuid& questSelection, const gte::uuid& objectiveSelection);
	void RenderHeader(NodeBuilder& builder, const char* name, const ImColor& color) const;
	void RenderInput(NodeBuilder& builder, const Pin& input) const;
	void RenderOutput(NodeBuilder& builder, const Pin& output) const;
	
	template<typename T>
	void RenderVariableNode(NodeBuilder& builder);

	[[nodiscard]] entt::entity FindEntity(ed::PinId pinId) const;


	template<typename T>
	[[nodiscard]] entt::entity SearchEntity(ed::NodeId nodeId) const;


	[[nodiscard]] Pin FindPin(entt::entity entityID, ed::PinId pinId) const;
	
	template<typename T>
	[[nodiscard]] Node* FindNode(entt::entity entityID) const;
	
	[[nodiscard]] Link* FindLink(ed::LinkId linkID);
	[[nodiscard]] Link* FindLink(ed::PinId pinID);
	
	//[[nodiscard]] const Pin FindPin(entt::entity entityID, ed::PinId pinId) const;
	
	[[nodiscard]] bool IsPinLinked(ed::PinId id) const;
	[[nodiscard]] bool CanCreateLink(const Pin& a, const Pin& b) const;

	float GetTouchProgress(ed::NodeId id);
	//template<typename T>
	//void BuildNode(const T& node);
	//
	//template<typename T, typename ...Args>
	//void BuildNodes(void);

	int GetNextID(void) { return mNextID++; }
	void ResetID(int id) { mNextID = id; }

private:
	size_t mID;
	entt::registry mECS;

	ed::NodeId mContextNodeId;
	ed::LinkId mContextLinkId;
	ed::PinId  mContextPinId;
	std::map<ed::NodeId, float, NodeIdLess> mNodeTouchTime;
	bool mCreateNewNode = false;
	Pin* mNewNodeLinkPin = nullptr;
	Pin* mNewLinkPin = nullptr;
	int32_t mNextID = 1;
	entt::entity mOpenActNode = entt::null;
	entt::entity mOpenAcceptQuest = entt::null;
	std::pair<entt::entity, int32_t> mOpenExpression = { entt::null, -1 };
	
	static constexpr float sTouchTime = 1.0f;

	static entt::registry sQuestECS;
	static size_t sNextID;

	friend class SceneSerializer;
	friend class ExportSerializer;
};

#include <Character.hpp>