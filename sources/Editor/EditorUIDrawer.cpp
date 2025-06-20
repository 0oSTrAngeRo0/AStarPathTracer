#include "Editor/EditorUIDrawer.h"
#include "Editor/UI/Inspectors/EditorInspector.h"
#include "imgui.h"
#include "Editor/UI/ImGuiFileDialog.h"
#include "Editor/UI/ResourcesPanel.h"
#include "Editor/UI/HierarchiesPanel.h"
#include "Editor/UI/ViewportPanel.h"
#include "Editor/EditorSelection.h"
#include "Utilities/EnumX.h"

EditorUIDrawer::EditorUIDrawer() {
    resources_panel = std::make_unique<ResourcesPanel>();
    selection = std::make_unique<EditorSelection>();
    hierachies_panel = std::make_unique<HierarchiesPanel>();
}

void EditorUIDrawer::DrawUI(entt::registry& registry) {
    ImGui::DockSpaceOverViewport();

    //ImGui::ShowDemoWindow();

    if (TryChangeSelection(resources_panel->DrawUi(TryGetSelectionID(SelectionType::eResources)), SelectionType::eResources)) {
        if (is_selection_leaf) {
            selection->SelectResource(selection_id);
        }
    }
    if (TryChangeSelection(hierachies_panel->DrawUi(registry, TryGetSelectionID(SelectionType::eHierachies)), SelectionType::eHierachies)) {
        TreeView::NodeId entity_str = selection_id;
        entt::entity entity = static_cast<entt::entity>(std::stoul(entity_str));
        selection->SelectEntity(entity, registry);
    }
    selection->DrawUi();
    // viewport_panel->DrawUi();

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

bool EditorUIDrawer::TryChangeSelection(TreeView::Result result, EditorUIDrawer::SelectionType type) {
    if (!result.clicked || result.mouse_button != ImGuiMouseButton_Left) return false;
    selection_type = type;
    selection_id = result.clicked.value().get().id;
    is_selection_leaf = result.clicked.value().get().is_leaf;
    ASTAR_PRINT("Selection Changed: [%s]\n", selection_id.c_str());
    return true;
}