#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <utility>

#include <Components.h>

#include <vector>
#include <string>

static inline ImRect ImGui_GetItemRect()
{
    return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
{
    auto result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}

namespace util = ax::NodeEditor::Utilities;
namespace ed = ax::NodeEditor;

void DrawPinIcon(const Pin& pin, bool connected, int alpha);