#include "Editor/EditorUIDrawer.h"
#include "Editor/UI/Inspectors/EditorInspector.h"
#include "imgui.h"
#include "Editor/UI/ImGuiFileDialog.h"
#include "Editor/UI/ResourcesPanel.h"
#include "Editor/UI/HierarchiesPanel.h"
#include "Editor/EditorSelection.h"


EditorUIDrawer::EditorUIDrawer() {
    resources_panel = std::make_unique<ResourcesPanel>();
    selection = std::make_unique<EditorSelection>();
    hierachies_panel = std::make_unique<HierachiesPanel>();
}

void EditorUIDrawer::DrawUI(entt::registry& registry) {
    ImGui::DockSpaceOverViewport();

    //ImGui::ShowDemoWindow();

    resources_panel->DrawUi();
    hierachies_panel->DrawUi(registry);

    ImGui::Begin("Inspector");
    if (resources_panel->IsSelectionChanged()) {
        auto [is_leaf, path] = resources_panel->GetCurrentSelection();
        if (is_leaf) {
            selection->SelectResource(path);
        }
        printf("Selection Changed: [%s]\n", path.c_str());
    }
    selection->GetSelectedInspector().DrawInspector();
    ImGui::End();

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action
        }

        // close
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            if (ImGui::MenuItem("Close")) {
                std::printf("Cloase");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

EditorUIDrawer::~EditorUIDrawer() {}
