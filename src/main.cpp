#include <Application.h>
#include <Scene.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include <Nodes.hpp>

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

static Scene* ActiveScene = nullptr;
static ImTextureID sHeaderBackground;
static ImTextureID sSaveIcon;
static ImTextureID sRestoreIcon;

//static void DrawItemRect(ImColor color, float expand = 0.0f)
//{
//    ImGui::GetWindowDrawList()->AddRect(
//        ImGui::GetItemRectMin() - ImVec2(expand, expand),
//        ImGui::GetItemRectMax() + ImVec2(expand, expand),
//        color);
//};

//static void FillItemRect(ImColor color, float expand = 0.0f, float rounding = 0.0f)
//{
//    ImGui::GetWindowDrawList()->AddRectFilled(
//        ImGui::GetItemRectMin() - ImVec2(expand, expand),
//        ImGui::GetItemRectMax() + ImVec2(expand, expand),
//        color, rounding);
//};


const char* Application_GetName()
{
    return "PuruPuru";
}

void Application_Initialize()
{
    

    //config.SettingsFile = "Blueprints.json";
    //
    //config.LoadNodeSettings = [](ed::NodeId nodeId, char* data, void* userPointer) -> size_t
    //{
    //    auto node = FindNode(nodeId);
    //    if (!node)
    //        return 0;
    //
    //    if (data != nullptr)
    //        memcpy(data, node->State.data(), node->State.size());
    //    return node->State.size();
    //};
    //
    //config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
    //{
    //    auto node = FindNode(nodeId);
    //    if (!node)
    //        return false;
    //
    //    node->State.assign(data, size);
    //
    //    TouchNode(nodeId);
    //
    //    return true;
    //};    

    ActiveScene = new Scene();
    
    sHeaderBackground = Application_LoadTexture("Data/BlueprintBackground.png");
    sSaveIcon = Application_LoadTexture("Data/ic_save_white_24dp.png");
    sRestoreIcon = Application_LoadTexture("Data/ic_restore_white_24dp.png");


    auto& io = ImGui::GetIO();
    io.WantCaptureKeyboard = false;
}


void Application_Finalize()
{
    auto releaseTexture = [](ImTextureID& id)
    {
        if (id)
        {
            Application_DestroyTexture(id);
            id = nullptr;
        }
    };

    releaseTexture(sRestoreIcon);
    releaseTexture(sSaveIcon);
    releaseTexture(sHeaderBackground);
    
    delete ActiveScene;
}

void Application_Frame()
{
    ActiveScene->RenderFrame();

    //ImGui::ShowDemoWindow();
    //ImGui::ShowTestWindow();
    //ImGui::ShowMetricsWindow();
}
