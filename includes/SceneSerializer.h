#pragma once

#include "Scene.h"

//Forward Decleration(s)
namespace YAML { class Emitter; }

class SceneSerializer {
public:
	SceneSerializer(Scene* scene);

	void Serialize(const std::string& filepath);
	void Deserialize(const std::string& filepath);

private:

	//void SerializeEntity(YAML::Emitter& out, entt::entity entity);

private:
	Scene* mScene = nullptr;
};