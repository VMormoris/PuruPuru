#include <Components.h>

YAML::Emitter& operator<<(YAML::Emitter& emitter, const std::array<Pin, 2>& pins) noexcept
{
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq;
	for (const auto& pin : pins)
		emitter << (uint64_t)pin.ID.AsPointer();
	emitter << YAML::EndSeq;
	return emitter;
}

YAML::Emitter& operator<<(YAML::Emitter& emitter, const std::vector<Pin>& pins) noexcept
{
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq;
	for (const auto& pin : pins)
		emitter << (uint64_t)pin.ID.AsPointer();
	emitter << YAML::EndSeq;
	return emitter;
}

YAML::Emitter& operator<<(YAML::Emitter& emitter, const ImVec2& vec) noexcept
{
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq;
	emitter << vec.x << vec.y;
	emitter << YAML::EndSeq;
	return emitter;
}