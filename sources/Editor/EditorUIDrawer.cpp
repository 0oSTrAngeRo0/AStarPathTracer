#include "Editor/EditorUIDrawer.h"
#include "Editor/UI/FileBrowser.h"

#include "imgui.h"

EditorUIDrawer::EditorUIDrawer() {
    assets_browser = std::make_unique<FileBrowser>("D:/C++/Projects/PathTracer/EngineRuntime/Resources");
}

void EditorUIDrawer::DrawUI(entt::registry& registry) {
    ImGuiWindowFlags window_flags = 0;
    bool open = true;
    
    // We specify a default position/size in case there's no data in the .ini file.
    // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    // Main body of the Demo window starts here.
    if (!ImGui::Begin("Dear ImGui Demo", &open, window_flags)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    assets_browser->OnDrawUi();
    ImGui::End();

}
