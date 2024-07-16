#include "Editor/EditorUIDrawer.h"
#include "Editor/UI/Inspectors/EditorInspector.h"
#include "imgui.h"
#include "Editor/UI/ImGuiFileDialog.h"


EditorUIDrawer::EditorUIDrawer() {
    resources_panel = std::make_unique<ResourcesPanel>();
    selection = std::make_unique<EditorSelection>();
}

void EditorUIDrawer::DrawUI(entt::registry& registry) {
    ImGui::ShowDemoWindow();
    ImGui::Begin("Inspector");
    selection->GetSelectedInspector().DrawInspector();
    ImGui::End();

    resources_panel->DrawUi();

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }
}
