#include "Editor/UI/ResourcesPanel.h"
#include <imgui.h>
#include <Editor/UI/ImGuiFileDialog.h>
#include "Engine/Resources/Resources.h"

ResourcesPanel::ResourcesPanel() {
    browser = std::make_unique<FileBrowser>(RESOURCES_DIR);
}

void ResourcesPanel::DrawUi() {
    ImGui::Begin("Resources");
    if (ImGui::Button("Import File")) {
        IGFD::FileDialogConfig config;
        config.path = ASSET_SOURCES_DIR;
        config.countSelectionMax = 1;
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose Resource File", ".*", config);
    }
    browser->OnDrawUi();
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
}
