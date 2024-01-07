#include "PuruPuru.h"

//io
#include <yaml-cpp/yaml.h>
#include <io/io-utils.h>

//gui
#include <gui/Image.h>
#include <gui/embedded-icons.h>
#include <gui/gui-utils.h>
#include <gui/widgets.h>

//Node Editor
#include <platform.h>

//GLFW
#include <GLFW/glfw3.h>

//std
#include <fstream>
#include <filesystem>

static gte::Image sMinimize;
static gte::Image sMaximize;
static gte::Image sClose;
static gte::Image sRestore;
static gte::Image sLogo;

PuruPuru* PuruPuru::sInstance = nullptr;

void PuruPuru::OnStart(void)
{
    if (std::filesystem::exists("default.ptheme"))
    {
        auto& io = ImGui::GetIO();
        const auto node = YAML::LoadFile("default.ptheme");
        const ImGuiStyle style = node.as<ImGuiStyle>();
        SetTheme(style);
    }

	GLFWwindow* window = (GLFWwindow*)m_Platform->GetPlatformAgnosticWindowHandle();
	glfwSetTitlebarHitTestCallback(window, [](GLFWwindow* window, int x, int y, int* hit)
	{
		*hit = sInstance->IsTitlebarHovered();
	});

}

void PuruPuru::OnStop(void)
{
	const auto& theme =ImGui::GetStyle();
	YAML::Emitter out;
	out << theme;

    std::ofstream os("default.ptheme", std::ios_base::binary);
    os << out.c_str();
    os.close();
}

void PuruPuru::OnFrame(float deltaTime)
{
    DrawMainWindow();
	DrawThemeWindow();
}

void PuruPuru::DrawThemeWindow(void)
{
    ImGui::Begin("Theme");
    auto& style = ImGui::GetStyle();
    gui::DrawTreeControl("General", [&]() {
        gui::UISettings<float> floatSettings;
        gui::UISettings<ImVec2> vec2Settings;

        gui::UISettings<ImVec4> colorSettings;
        colorSettings.Max = { 1.0f, 1.0f, 1.0f, 1.0f };
        colorSettings.Speed = { 0.05f, 0.05f, 0.05f, 0.05f };

        gui::DrawControlRow("Checkmark", style.Colors[ImGuiCol_CheckMark], colorSettings);
        gui::DrawControlRow("Menu-Bar background", style.Colors[ImGuiCol_MenuBarBg], colorSettings);
        gui::DrawControlRow("Mouse Scale", style.MouseCursorScale, floatSettings);

        gui::DrawTableSeparator(2);

        gui::DrawControlRow("Text", style.Colors[ImGuiCol_Text], colorSettings);
        gui::DrawControlRow("Text disabled", style.Colors[ImGuiCol_TextDisabled], colorSettings);
        gui::DrawTableSeparator(2);

        gui::DrawControlRow("Border", style.Colors[ImGuiCol_Border], colorSettings);
        gui::DrawControlRow("Border Shadow", style.Colors[ImGuiCol_BorderShadow], colorSettings);
        gui::DrawTableSeparator(2);

        gui::DrawControlRow("Seperator", style.Colors[ImGuiCol_Separator], colorSettings);
        gui::DrawControlRow("Seperator hovered", style.Colors[ImGuiCol_SeparatorHovered], colorSettings);
        gui::DrawControlRow("Seperator active", style.Colors[ImGuiCol_SeparatorActive], colorSettings);

        gui::DrawControlRow("Seperator text alignment", style.SeparatorTextAlign, vec2Settings);
        gui::DrawControlRow("Seperator text padding", style.SeparatorTextPadding, vec2Settings);
        gui::DrawControlRow("Seperator text border size", style.SeparatorTextBorderSize, floatSettings);

        gui::DrawTableSeparator(2);

        gui::DrawControlRow("Item spacing", style.ItemSpacing, vec2Settings);
        gui::DrawControlRow("Item inner spacing", style.ItemInnerSpacing, vec2Settings);
        gui::DrawTableSeparator(2);

        floatSettings.Max = 1.0f;
        floatSettings.Speed = 0.05f;
        gui::DrawControlRow("Global Aplha", style.Alpha, floatSettings);
        gui::DrawControlRow("Disabled Aplha", style.DisabledAlpha, floatSettings);
    });

    gui::DrawTreeControl("Windows", [&]() {
        gui::UISettings<float> floatSettings;
        gui::UISettings<ImVec2> vec2Settings;

        gui::UISettings<ImVec4> colorSettings;
        colorSettings.Max = { 1.0f, 1.0f, 1.0f, 1.0f };
        colorSettings.Speed = { 0.05f, 0.05f, 0.05f, 0.05f };

        gui::DrawControlRow("Background", style.Colors[ImGuiCol_WindowBg], colorSettings);
        gui::DrawControlRow("Padding", style.WindowPadding, vec2Settings);
        gui::DrawControlRow("Minimum size", style.WindowMinSize, vec2Settings);
        //Add WindowMenuButtonPosition
        gui::DrawControlRow("Rounding", style.WindowRounding, floatSettings);
        gui::DrawControlRow("Border size", style.WindowBorderSize, floatSettings);
        gui::DrawTableSeparator(2);

        gui::DrawControlRow("Title BG", style.Colors[ImGuiCol_TitleBg], colorSettings);
        gui::DrawControlRow("Title BG Active", style.Colors[ImGuiCol_TitleBgActive], colorSettings);
        gui::DrawControlRow("Title BG Collapsed", style.Colors[ImGuiCol_TitleBgCollapsed], colorSettings);
        gui::DrawControlRow("Title alignment", style.WindowTitleAlign, vec2Settings);
        gui::DrawTableSeparator(2);

        gui::DrawControlRow("Resize-Grip", style.Colors[ImGuiCol_ResizeGrip], colorSettings);
        gui::DrawControlRow("Resize-Grip hovered", style.Colors[ImGuiCol_ResizeGripHovered], colorSettings);
        gui::DrawControlRow("Resize-Grip active", style.Colors[ImGuiCol_ResizeGripActive], colorSettings);
    });

    gui::DrawTreeControl("Child Windows", [&]() {
        gui::UISettings<float> floatSettings;

        gui::UISettings<ImVec4> colorSettings;
        colorSettings.Max = { 1.0f, 1.0f, 1.0f, 1.0f };
        colorSettings.Speed = { 0.05f, 0.05f, 0.05f, 0.05f };

        gui::DrawControlRow("Background", style.Colors[ImGuiCol_ChildBg], colorSettings);

        gui::DrawControlRow("Rounding", style.ChildRounding, floatSettings);
        gui::DrawControlRow("Border size", style.ChildBorderSize, floatSettings);
    });

    gui::DrawTreeControl("Popup Windows", [&]() {
        gui::UISettings<float> floatSettings;

        gui::UISettings<ImVec4> colorSettings;
        colorSettings.Max = { 1.0f, 1.0f, 1.0f, 1.0f };
        colorSettings.Speed = { 0.05f, 0.05f, 0.05f, 0.05f };

        gui::DrawControlRow("Background", style.Colors[ImGuiCol_PopupBg], colorSettings);
        gui::DrawControlRow("Diming", style.Colors[ImGuiCol_ModalWindowDimBg], colorSettings);

        gui::DrawControlRow("Rounding", style.PopupRounding, floatSettings);
        gui::DrawControlRow("PopupBorderSize", style.PopupBorderSize, floatSettings);
    });

    gui::DrawTreeControl("Scrollbars", [&]() {
        gui::UISettings<float> floatSettings;

        gui::UISettings<ImVec4> colorSettings;
        colorSettings.Max = { 1.0f, 1.0f, 1.0f, 1.0f };
        colorSettings.Speed = { 0.05f, 0.05f, 0.05f, 0.05f };

        gui::DrawControlRow("Background", style.Colors[ImGuiCol_ScrollbarBg], colorSettings);
        gui::DrawControlRow("Scrollbar grab", style.Colors[ImGuiCol_ScrollbarGrab], colorSettings);
        gui::DrawControlRow("Scrollbar hovered", style.Colors[ImGuiCol_ScrollbarGrabHovered], colorSettings);
        gui::DrawControlRow("Scrollbar active", style.Colors[ImGuiCol_ScrollbarGrab], colorSettings);

        gui::DrawControlRow("Rounding", style.ScrollbarRounding, floatSettings);
        gui::DrawControlRow("Size", style.ScrollbarSize, floatSettings);

        gui::DrawControlRow("Grab rounding", style.GrabRounding, floatSettings);
        gui::DrawControlRow("Minimum grab size", style.GrabMinSize, floatSettings);
    });

    gui::DrawTreeControl("Tabs", [&]() {
        gui::UISettings<float> floatSettings;

        gui::UISettings<ImVec4> colorSettings;
        colorSettings.Max = { 1.0f, 1.0f, 1.0f, 1.0f };
        colorSettings.Speed = { 0.05f, 0.05f, 0.05f, 0.05f };

        gui::DrawControlRow("Background", style.Colors[ImGuiCol_Tab], colorSettings);
        gui::DrawControlRow("Hovered", style.Colors[ImGuiCol_TabHovered], colorSettings);
        gui::DrawControlRow("Active", style.Colors[ImGuiCol_TabActive], colorSettings);
        gui::DrawControlRow("Unfocused", style.Colors[ImGuiCol_TabUnfocused], colorSettings);
        gui::DrawControlRow("Unfocused & Active", style.Colors[ImGuiCol_TabUnfocusedActive], colorSettings);

        gui::DrawControlRow("Rounding", style.TabRounding, floatSettings);
        gui::DrawControlRow("Border size", style.TabBorderSize, floatSettings);
        gui::DrawControlRow("Min width close", style.TabMinWidthForCloseButton, floatSettings);
        gui::DrawControlRow("Bar border size", style.TabBarBorderSize, floatSettings);
    });

    gui::DrawTreeControl("Widgets", [&]() {
        gui::UISettings<ImVec4> colorSettings;
        colorSettings.Max = { 1.0f, 1.0f, 1.0f, 1.0f };
        colorSettings.Speed = { 0.05f, 0.05f, 0.05f, 0.05f };

        gui::UISettings<ImVec2> vec2Settings;
        gui::UISettings<float> floatSettings;

        gui::DrawControlRow("Background", style.Colors[ImGuiCol_FrameBg], colorSettings);
        gui::DrawControlRow("Hovered", style.Colors[ImGuiCol_FrameBgHovered], colorSettings);
        gui::DrawControlRow("Active", style.Colors[ImGuiCol_FrameBgActive], colorSettings);

        gui::DrawControlRow("Padding", style.FramePadding, vec2Settings);

        gui::DrawControlRow("Border radius", style.FrameRounding, floatSettings);
        gui::DrawControlRow("Border size", style.FrameBorderSize, floatSettings);
    });

    gui::DrawTreeControl("Selectables", [&]() {
        gui::UISettings<ImVec4> colorSettings;
        colorSettings.Max = { 1.0f, 1.0f, 1.0f, 1.0f };
        colorSettings.Speed = { 0.05f, 0.05f, 0.05f, 0.05f };

        gui::UISettings<float> floatSettings;
        gui::UISettings<ImVec2> vec2Settings;

        gui::DrawControlRow("Background", style.Colors[ImGuiCol_Header], colorSettings);
        gui::DrawControlRow("Hovered", style.Colors[ImGuiCol_HeaderHovered], colorSettings);
        gui::DrawControlRow("Active", style.Colors[ImGuiCol_HeaderActive], colorSettings);
        gui::DrawControlRow("Text", style.Colors[ImGuiCol_TextSelectedBg], colorSettings);

        gui::DrawControlRow("Text alignment", style.SelectableTextAlign, vec2Settings);

        gui::DrawControlRow("Indent", style.IndentSpacing, floatSettings);
    });

    gui::DrawTreeControl("Buttons", [&]() {
        gui::UISettings<ImVec2> vec2Settings;

        gui::UISettings<ImVec4> colorSettings;
        colorSettings.Max = { 1.0f, 1.0f, 1.0f, 1.0f };
        colorSettings.Speed = { 0.05f, 0.05f, 0.05f, 0.05f };

        gui::DrawControlRow("Background", style.Colors[ImGuiCol_Button], colorSettings);
        gui::DrawControlRow("Hovered", style.Colors[ImGuiCol_ButtonHovered], colorSettings);
        gui::DrawControlRow("Active", style.Colors[ImGuiCol_ButtonActive], colorSettings);

        gui::DrawControlRow("Text allignment", style.ButtonTextAlign, vec2Settings);
    });

    ImGui::End();
}

void PuruPuru::DrawMainWindow(void)
{
    {
        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        //if (!m_Specification.CustomTitlebar && m_MenubarCallback)
		//	window_flags |= ImGuiWindowFlags_MenuBar;

        const bool isMaximized = IsMaximized();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);

        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::Begin("DockSpaceWindow", nullptr, window_flags);
        ImGui::PopStyleColor(); // MenuBarBg
        ImGui::PopStyleVar(2);

		auto* window = ImGui::GetCurrentWindow();
		ImVec2 size, pos;
		gui::UpdateWindowManualResize(window, size, pos);
        
		ImGui::PopStyleVar(2);

        {
            ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(50, 50, 50, 255));
            // Draw window border if the window is not maximized
            if (!isMaximized)
                gui::RenderWindowOuterBorders(ImGui::GetCurrentWindow());
            ImGui::PopStyleColor(); // ImGuiCol_Border
        }

        //if (m_Specification.CustomTitlebar)
        //{
            float titleBarHeight;
            DrawCustomTitleBar(titleBarHeight);
            ImGui::SetCursorPosY(titleBarHeight);
        //}

        // Dockspace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
        ImGui::DockSpace(ImGui::GetID("MyDockspace"));
        style.WindowMinSize.x = minWinSizeX;
        //if (!m_Specification.CustomTitlebar)
        //    UI_DrawMenubar();
        ImGui::End();
    }
}

void PuruPuru::DrawCustomTitleBar(float& outTitlebarHeight)
{
    const float titlebarHeight = 58.0f;
	const bool isMaximized = IsMaximized();
	float titlebarVerticalOffset = isMaximized ? -6.0f : 0.0f;
	const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;

	ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + titlebarVerticalOffset));
	const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
	const ImVec2 titlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
		ImGui::GetCursorScreenPos().y + titlebarHeight };
	auto* bgDrawList = ImGui::GetBackgroundDrawList();
	auto* fgDrawList = ImGui::GetForegroundDrawList();

	const auto& theme = ImGui::GetStyle();
	ImU32 bg = (ImU32)ImColor{ theme.Colors[ImGuiCol_TitleBg] };
	bgDrawList->AddRectFilled(titlebarMin, titlebarMax, bg);
	// DEBUG TITLEBAR BOUNDS
	//fgDrawList->AddRect(titlebarMin, titlebarMax, 1);

	// Logo
	{

		const int logoWidth = 48;// m_LogoTex->GetWidth();
		const int logoHeight = 48;// m_LogoTex->GetHeight();
		if (sLogo.Size() == 0)
			sLogo.Load("data/logo.png");
		const ImVec2 logoOffset(16.0f + windowPadding.x, 5.0f + windowPadding.y + titlebarVerticalOffset);
		const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
		const ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
		fgDrawList->AddImage(sLogo.GetHandle(), logoRectStart, logoRectMax);
	}

	ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - windowPadding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

	static float moveOffsetX;
	static float moveOffsetY;
	const float w = ImGui::GetContentRegionAvail().x;
	const float buttonsAreaWidth = 94;

	// Title bar drag area
	// On Windows we hook into the GLFW win32 window internals
	ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y + titlebarVerticalOffset)); // Reset cursor pos
	// DEBUG DRAG BOUNDS
	//const ImColor debugColor = { 255, 0, 0, 255 };
	//fgDrawList->AddRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + w - buttonsAreaWidth, ImGui::GetCursorScreenPos().y + titlebarHeight), debugColor);
	ImGui::InvisibleButton("##titleBarDragZone", ImVec2(w - buttonsAreaWidth, titlebarHeight));

	mTitlebarHovered = ImGui::IsItemHovered();

	if (isMaximized)
	{
		float windowMousePosY = ImGui::GetMousePos().y - ImGui::GetCursorScreenPos().y;
		if (windowMousePosY >= 0.0f && windowMousePosY <= 5.0f)
			mTitlebarHovered = true; // Account for the top-most pixels which don't register
	}

	// Draw Menubar
	{
		ImGui::SuspendLayout();
		{
			ImGui::SetItemAllowOverlap();
			const float logoHorizontalOffset = 16.0f * 2.0f + 48.0f + windowPadding.x;
			ImGui::SetCursorPos(ImVec2(logoHorizontalOffset, 6.0f + titlebarVerticalOffset));
			DrawMenubar();
	
			if (ImGui::IsItemHovered())
				mTitlebarHovered = false;
		}
		ImGui::ResumeLayout();
	}

	{
		// Centered Window title
		ImVec2 currentCursorPos = ImGui::GetCursorPos();
		const auto& appname = GetName();
		ImVec2 textSize = ImGui::CalcTextSize(appname.c_str());
		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() * 0.5f - textSize.x * 0.5f, 2.0f + windowPadding.y + 6.0f));
		ImGui::Text(appname.c_str()); // Draw title
		ImGui::SetCursorPos(currentCursorPos);
	}

	// Window buttons
	const auto& buttonCol = theme.Colors[ImGuiCol_Text];
	const ImU32 buttonColN = (ImU32)ImColor{ ImVec4 {
		buttonCol.x * 0.9f,
		buttonCol.y * 0.9f,
		buttonCol.z * 0.9f,
		buttonCol.w * 0.9f
	}};
	const ImU32 buttonColH = (ImU32)ImColor{ ImVec4 {
		buttonCol.x * 1.2f,
		buttonCol.y * 1.2f,
		buttonCol.z * 1.2f,
		buttonCol.w * 1.2f
	} };
	const ImU32 buttonColP = (ImU32)ImColor{ theme.Colors[ImGuiCol_TextDisabled] };
	const float buttonWidth = 14.0f;
	const float buttonHeight = 14.0f;

	// Minimize Button
	ImGui::Spring();
	gui::ShiftCursorY(8.0f);
	{
		if (sMinimize.Size() == 0)
			sMinimize.Load(gWindowMinimizeIcon, sizeof(gWindowMinimizeIcon));
		const int iconWidth = sMinimize.GetWidth();
		const int iconHeight = sMinimize.GetHeight();
		const float padY = (buttonHeight - (float)iconHeight) / 2.0f;
		if (ImGui::InvisibleButton("Minimize", ImVec2(buttonWidth, buttonHeight)))
		{
			auto* window = (GLFWwindow*)m_Platform->GetPlatformAgnosticWindowHandle();
			glfwIconifyWindow(window);
		}
		gui::DrawButtonImage(sMinimize.GetHandle(), buttonColN, buttonColH, buttonColP, gui::RectExpanded(gui::GetItemRect(), { 0.0f, -padY }));
	}


	// Maximize Button
	ImGui::Spring(-1.0f, 17.0f);
	gui::ShiftCursorY(8.0f);
	{
		if (sMaximize.Size() == 0)
			sMaximize.Load(gWindowMaximizeIcon, sizeof(gWindowMaximizeIcon));
		if (sRestore.Size() == 0)
			sRestore.Load(gWindowRestoreIcon, sizeof(gWindowRestoreIcon));
		const int iconWidth = sMaximize.GetWidth();
		const int iconHeight = sMaximize.GetHeight();
		if (ImGui::InvisibleButton("Maximize", ImVec2(buttonWidth, buttonHeight)))
		{
			GLFWwindow* window = (GLFWwindow*)m_Platform->GetPlatformAgnosticWindowHandle();
			if (isMaximized)
				glfwRestoreWindow(window);
			else
				glfwMaximizeWindow(window);
		}

		gui::DrawButtonImage(
			isMaximized ? sRestore.GetHandle() : sMaximize.GetHandle(),
			buttonColN, buttonColH, buttonColP
		);
	}

	// Close Button
	ImGui::Spring(-1.0f, 15.0f);
	gui::ShiftCursorY(8.0f);
	{
		if (sClose.Size() == 0)
			sClose.Load(gWindowCloseIcon, sizeof(gWindowCloseIcon));
		const int iconWidth = sClose.GetWidth();
		const int iconHeight = sClose.GetHeight();
		const auto& color = theme.Colors[ImGuiCol_Text];
		const ImU32 baseColor = ImColor{
			color.x * 0.9f,
			color.y * 0.9f,
			color.z * 0.9f,
			color.w * 0.9f,
		};
		const ImU32 hoveredColor = ImColor{ ImVec4{
			color.x * 1.4f,
			color.y * 1.4f,
			color.z * 1.4f,
			color.w * 1.4f,
		} };

		if (ImGui::InvisibleButton("Close", ImVec2(buttonWidth, buttonHeight)))
            Close();
		gui::DrawButtonImage(sClose.GetHandle(), baseColor, hoveredColor, buttonColP);
	}

	ImGui::Spring(-1.0f, 18.0f);
	ImGui::EndHorizontal();

	outTitlebarHeight = titlebarHeight;
}

void PuruPuru::DrawMenubar(void)
{
	const ImRect menuBarRect = { ImGui::GetCursorPos(), { ImGui::GetContentRegionAvail().x + ImGui::GetCursorScreenPos().x, ImGui::GetFrameHeightWithSpacing() } };

	ImGui::BeginGroup();
	if (gui::BeginMenubar(menuBarRect))
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New", "Ctrl+N");
			ImGui::MenuItem("Open...", "Ctrl+O");
			ImGui::MenuItem("Save as...", "Ctrl+Shift+S");
			ImGui::MenuItem("Save", "Ctrl+S");
			ImGui::Separator();
			ImGui::MenuItem("Export");
			ImGui::MenuItem("Export lines");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Theme"))
		{
			ImGui::MenuItem("Import");
			ImGui::MenuItem("Export");
			ImGui::EndMenu();
		}
	}

	gui::EndMenubar();
	ImGui::EndGroup();
}

bool PuruPuru::IsMaximized(void) const
{
	GLFWwindow* window = (GLFWwindow*)m_Platform->GetPlatformAgnosticWindowHandle();
	const auto flag = (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
	return flag;
}

int Main(int argc, char** argv)
{
    PuruPuru app("PuruPuru v0.2.0");

    if (app.Create())
        return app.Run();

    return 0;
}