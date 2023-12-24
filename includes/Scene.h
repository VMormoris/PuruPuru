#pragma once

#include "Character.h"
#include "StateMachine.h"

#include <imgui_node_editor.h>

namespace ed = ax::NodeEditor;

class Scene {
	struct CharacterData{
		char Name[64] = "Unnamed Character";
		Character Self;
		ed::EditorContext* Editor = nullptr;
		Flavor CharacterFlavor = Flavor::Bitter;

		CharacterData(void) = default;
		CharacterData(ed::EditorContext* editor)
			: Editor(editor), Self({}) {}
	};

public:

	static constexpr size_t INVALID_ID = static_cast<size_t>(-1);
	static constexpr size_t CHARACTERS_INDEX = 0;
	static constexpr size_t NODE_EDITOR_INDEX = 1;
	static constexpr size_t ACT_INDEX = 2;

	static constexpr size_t EXPRESSION_INDEX = 3;
	static constexpr size_t DIALOGUE_INDEX = 4;
	static constexpr size_t VARIABLE_INDEX = 5;
	
	static constexpr size_t QUEST_INDEX = 6;
	static std::array<bool, 7> sWindows;

public:

	Scene(void);
	~Scene(void) noexcept;

	void Scene::RenderFrame(void) noexcept;

private:

	void ShowPanels();
	void SaveAs();
	void Save();
	void Open();

private:

	std::vector<CharacterData> mAllData;
	size_t mWorkingDataIndex = 0;
	size_t mEditingIndex = INVALID_ID;
	std::string mLastFilepath;
	bool mDebuging = false;
	bool mSpeaking = false;
	bool mDone = false;
	int32_t mChoice = -1;
	int32_t mDebugingID = 0;
	StateMachine mStateMachine;
	friend class SceneSerializer;
	friend class ExportSerializer;
};