#pragma once

#include "Scene.h"

//Forward Decleration(s)
namespace YAML { class Emitter; }

class ExportSerializer {
public:
	ExportSerializer(Scene* scene);

	void Serialize(const std::string& filepath);
	void SerializeLines(const std::string& filepath);

private:

    [[nodiscard]] std::vector<uint64_t> FindTargets(const entt::registry& reg, const Pin& pin)
	{
		const ed::NodeId INVALID_ID = ed::NodeId{ 0 };
		std::vector<uint64_t> data;
		auto view = reg.view<Link>();
		for (auto&& [entityID, link] : view.each())
		{
			if (link.StartPinID == pin.ID)
			{
				Node* node = FindNode(reg, link.EndPinID);
				if (node && node->ID != INVALID_ID)
					data.emplace_back((uint64_t)node->ID.AsPointer());
			}
		}

		if (data.empty())
			data.emplace_back(0);

		return data;
	}

	[[nodiscard]] Node* FindNode(const entt::registry& reg, ed::PinId pinId);

	template<typename T>
	[[nodiscard]] Node* FindNode(const entt::registry& reg, entt::entity entityID) const
	{
		if (auto* node = reg.try_get<T>(entityID))
			return (Node*)node;

		return nullptr;
	}

	template<typename T, typename ...Args>
	[[nodiscard]] Node* FindNodes(ComponentGroup<T, Args...>, const entt::registry& reg, entt::entity entityID) const
	{
		if (Node* node = FindNode<T>(reg, entityID))
			return node;

		if constexpr (sizeof...(Args) > 0)
			return FindNodes<Args...>(ComponentGroup<Args...>{}, reg, entityID);
		return nullptr;
	}

private:
	Scene* mScene = nullptr;
};