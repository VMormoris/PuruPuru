#pragma once

#include <imgui_node_editor.h>

#include <ax/Math2D.h>
#include <ax/Builders.h>
#include <ax/Widgets.h>

#include <entt/entt.hpp>

#include <yaml-cpp/emitter.h>

#include <array>
#include <uuid.h>

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

inline constexpr size_t STR_LENGTH = 64;

enum class PinType
{
    Flow,
    //Bool,
    //Int,
    //Float,
    //String,
    //Object,
    //Function,
    //Delegate,
};

enum class PinKind
{
    Output,
    Input
};

enum class NodeKind
{
    Blueprint,
    Simple,
    //Tree,
    Comment
};

enum class NodeType {
    None,
    Entry,
    Fork,
    BoolVariable,
    IntVariable,
    Branch,
    Dialogue,
    FlavorMatch,
    FlavorCheck,
    Act,
    Dice,
    AcceptQuest,
    ReturnQuest,
    Objective
};

struct Node;

struct Pin
{
    ed::PinId   ID = 0;
    std::string Name;
    PinType     Type = PinType::Flow;
    PinKind     Kind = PinKind::Input;

    Pin(void) = default;
    Pin(int id)
        : ID(id) {}
    Pin(int id, PinKind kind)
        : ID(id), Kind(kind) {}
    Pin(int id, const char* name, PinKind kind)
        : ID(id), Name(name), Kind(kind) {}
    Pin(int id, const char* name, PinType type)
        : ID(id), Name(name), Type(type) {}
    Pin(int id, const char* name, PinKind kind, PinType type)
        : ID(id), Name(name), Kind(kind), Type(type) {}
};

struct InputOutput {
    Pin Input;
    Pin Output;

    InputOutput(void) = default;
};

struct ForkInputOutput {
    Pin Input;
    std::array<Pin, 2> Outputs;

    ForkInputOutput(void) = default;
};

struct InputOutputs {
    Pin Input;
    std::vector<Pin> Outputs;

    InputOutputs(void) = default;
};

struct Node
{
    ed::NodeId ID = 0;
    NodeKind Kind = NodeKind::Blueprint;
    ImVec2 Size = { 0.0f, 0.0f };
    ImVec2 Position = { 0.0f, 0.0f };

    std::string State;
    std::string SavedState;

    Node(int id)
        : ID(id) { }

    virtual ~Node(void) = default;
};

struct CommentNode : public Node {

    std::string Comment;

    CommentNode(int id)
        : Node(id)
    {
        Kind = NodeKind::Comment;
    }
};

enum class SetOperator : int32_t{
    Assignment = 0,
    Add,
    Subtract,
    Multiple,
    Divide
};

template<typename T>
struct VariableNode : public Node {
    static const char NAME[STR_LENGTH];
    static const ImColor COLOR;
    
    char VariableName[STR_LENGTH] = "Your variable";
    SetOperator Operator = SetOperator::Assignment;
    T Value = 0;
    
    VariableNode(int id)
        : Node(id) { }

};

enum class CompareOperator : int32_t {
    Equality = 0,
    Greater,
    Less,
    GreaterEquals,
    LessEquals,
    Different
};

struct Condition{
    char VariableName[STR_LENGTH] = "Variable Name";
    CompareOperator Operator = CompareOperator::Equality;
    int32_t Value;
};

using Expression = std::vector<Condition>;

struct BranchNode : public Node {
    static const char NAME[STR_LENGTH];
    static const ImColor COLOR;
    std::vector<Expression> Expressions;

    BranchNode(int id)
        : Node(id) { }
};

struct DialogueNode : public Node {
    static const char NAME[STR_LENGTH];
    static const ImColor COLOR;

    std::vector<std::string> Prompts;

    DialogueNode(int id)
        : Node(id) { }
};

struct FlavorMatchNode : public Node {
    static const char NAME[STR_LENGTH];
    static const ImColor COLOR;

    FlavorMatchNode(int id)
        : Node(id) { }
};

enum class Flavor : int32_t {
    Bitter,
    Salty,
    Sour,
    Sweet,
    Neutral,

    //Cominations
    BitterSalty,
    BitterSweet,
    BitterSour,

    SaltySweet,
    SaltySour,

    SweetSour
};

struct FlavorCheckNode : public Node {
    static const char NAME[STR_LENGTH];
    static const ImColor COLOR;

    bool CheckingNPC = false;

    FlavorCheckNode(int id)
        : Node(id) { }
    FlavorCheckNode(int id, bool checkingNpc)
        : Node(id), CheckingNPC(checkingNpc) { }
};

struct ForkNode : public Node {
    static const char NAME[STR_LENGTH];
    static const ImColor COLOR;
    gte::uuid UUID;
    ForkNode(int id)
        : Node(id), UUID(gte::uuid::Create()) { }
    ForkNode(int id, const gte::uuid& uuid)
        : Node(id), UUID(uuid) { }
};

enum class Speaker : int32_t {
    MainCharacter = 0,
    NPC,
    Internal
};

struct ActNode : public Node {
    static const char NAME[64];
    static const ImColor COLOR;
    char Title[64] = "Your title";

    std::vector<std::pair<Speaker, std::string>> Bubbles;
    ActNode(int id)
        : Node(id) { }

};

struct DiceNode : public Node {
    static const char NAME[64];
    static const ImColor COLOR;
    
    DiceNode(int id)
        : Node(id) { }
};

struct ObjectiveSpecification {
    gte::uuid UUID = gte::uuid::Create();
    char Title[64] = "Your Title";
    char Description[512] = "Write the Objective's decription";
    bool IsOptional = false;
};

struct Character;

struct AcceptQuestNode : public Node {
    static const char NAME[64];
    static const ImColor COLOR;

    gte::uuid UUID;
    char Title[64] = "Your Title";
    char Description[512] = "Write the Quest's decription";
    size_t Owner;
    std::vector<ObjectiveSpecification> Objectives;

    AcceptQuestNode(int id)
        : Node(id) { }
};

struct ReturnQuestNode : public Node {
    static const char NAME[64];
    static const ImColor COLOR;

    gte::uuid QuestID;
    bool Succeed = true;

    ReturnQuestNode(int id)
        : Node(id) { }
};

struct ObjectiveNode : public Node {
    static const char NAME[64];
    static const ImColor COLOR;

    gte::uuid QuestID;
    gte::uuid ObjectiveID;
    bool Succeed = true;

    ObjectiveNode(int id)
        : Node(id) { }
};

struct Link
{
    static const ImColor COLOR;
    ed::LinkId ID;

    ed::PinId StartPinID;
    ed::PinId EndPinID;

    Link(ed::LinkId id)
        : ID(id) { }
    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId)
        : ID(id), StartPinID(startPinId), EndPinID(endPinId) { }

};

YAML::Emitter& operator<<(YAML::Emitter& emitter, const std::array<Pin, 2>& pins) noexcept;
YAML::Emitter& operator<<(YAML::Emitter& emitter, const std::vector<Pin>& pins) noexcept;
YAML::Emitter& operator<<(YAML::Emitter& emitter, const ImVec2& vec) noexcept;

template<typename ...T>
struct ComponentGroup{};

using AnyNode = ComponentGroup<
    AcceptQuestNode, ReturnQuestNode, ObjectiveNode,
    VariableNode<bool>, VariableNode<int32_t>,
    ActNode,
    ForkNode, BranchNode,
    DialogueNode,
    FlavorMatchNode, FlavorCheckNode,
    DiceNode,
    CommentNode,
    Node
>;

#include <Components.hpp>