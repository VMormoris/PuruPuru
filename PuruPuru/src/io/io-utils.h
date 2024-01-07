#pragma once

#include <yaml-cpp/emitter.h>

#include <imgui.h>

YAML::Emitter& operator<<(YAML::Emitter& emitter, const ImVec2& vec) noexcept;
YAML::Emitter& operator<<(YAML::Emitter& emitter, const ImVec4& vec) noexcept;
YAML::Emitter& operator<<(YAML::Emitter& emitter, const ImGuiStyle& theme) noexcept;

#include "io-utils.hpp"