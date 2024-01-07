#pragma once

#include <imgui_internal.h>

namespace gui {

	void ShiftCursorX(float distance);

	void ShiftCursorY(float distance);

	void ShiftCursor(const ImVec2& offset);

	ImRect GetItemRect(void);

	ImRect RectExpanded(const ImRect& rect, const ImVec2& size);

	ImRect RectOffset(const ImRect& rect, const ImVec2& offset);

	void DrawButtonImage(const ImTextureID id,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImVec2 rectMin, ImVec2 rectMax);

	void DrawButtonImage(const ImTextureID id,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImRect rectangle);

	void DrawButtonImage(const ImTextureID id,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed);

	void DrawTitlebar(float& outTitlebarHeight);

	void RenderWindowOuterBorders(ImGuiWindow* window);

	// Exposed resize behavior for native OS windows
	bool UpdateWindowManualResize(ImGuiWindow* window, ImVec2& newSize, ImVec2& newPosition);

	bool BeginMenubar(const ImRect& barRectangle);
	void EndMenubar(void);

}