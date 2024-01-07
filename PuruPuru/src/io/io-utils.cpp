#include "io-utils.h"

YAML::Emitter& operator<<(YAML::Emitter& emitter, const ImVec2& vec) noexcept
{
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq;
	emitter << vec.x << vec.y;
	emitter << YAML::EndSeq;
	return emitter;
}

YAML::Emitter& operator<<(YAML::Emitter& emitter, const ImVec4& vec) noexcept
{
	emitter << YAML::Flow;
	emitter << YAML::BeginSeq;
	emitter << vec.x << vec.y << vec.z << vec.w;
	emitter << YAML::EndSeq;
	return emitter;
}

YAML::Emitter& operator<<(YAML::Emitter& emitter, const ImGuiStyle& theme) noexcept
{
	emitter << YAML::convert<ImGuiStyle>::encode(theme);
	return emitter;
}