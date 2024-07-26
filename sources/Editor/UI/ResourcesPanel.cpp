#include "Editor/UI/ResourcesPanel.h"
#include <imgui.h>
#include <Editor/UI/ImGuiFileDialog.h>
#include "Engine/Resources/Resources.h"
#include <print>

std::vector<ResourcesPanel::CreateResourceMenuItemNode> ResourcesPanel::creaet_resource_data = {
	{
		"Material",
		std::vector<ResourcesPanel::CreateResourceMenuItemNode>{
			{
				"SimpleLit", []() { std::println("Create Material"); }
			}
		}
	}
};

ResourcesPanel::ResourcesPanel() {
	browser = std::make_unique<FileBrowser>(RESOURCES_DIR);
}

void ResourcesPanel::DrawCraetePopupNode(const CreateResourceMenuItemNode& node) {
	std::visit([&](auto&& arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, std::function<void()>>) {
			if (ImGui::MenuItem(node.label.c_str())) {
				std::println("Label [{0}] is clicked", node.label);
				arg();
			}
		} else if constexpr (std::is_same_v<T, std::vector<CreateResourceMenuItemNode>>) {
			if (ImGui::BeginMenu(node.label.c_str())) {
				for (const CreateResourceMenuItemNode& child : arg) {
					DrawCraetePopupNode(child);
				}
				ImGui::EndMenu();
			}
		}
	}, node.data);
}

void ResourcesPanel::DrawCreatePopup() {
	auto& state = browser->GetCurrentState();
	if (state.is_changed && state.is_directory && state.mouse_button == ImGuiMouseButton_Right) {
		ImGui::OpenPopup("Create Resource");
	}
	if (ImGui::BeginPopup("Create Resource")) {
		for (const CreateResourceMenuItemNode& child : creaet_resource_data) {
			DrawCraetePopupNode(child);
		}
		ImGui::EndPopup();
	}
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
	DrawCreatePopup();
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
