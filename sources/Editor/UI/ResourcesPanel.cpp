#include "Engine/Resources/ResourcesManager.h"
#include "Editor/UI/ResourcesPanel.h"
#include <imgui.h>
#include <Editor/UI/ImGuiFileDialog.h>
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourceData.h"

ResourcesPanel::ResourcesPanel() {
	root = TreeView::CreateDirectryNodeTreeFromPath(ResourcesManager::GetInstance().GetResourcesDirectory());
}


void ResourcesPanel::DrawCraetePopupNode(const ResourceCreateMenuRegistry::Node& node) {
	std::visit([&](auto&& arg) {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, ResourceCreateMenuRegistry::Leaf>) {
			if (ImGui::MenuItem(std::string(node.key).c_str())) {
				arg(current_state.id);
				root = TreeView::CreateDirectryNodeTreeFromPath(ResourcesManager::GetInstance().GetResourcesDirectory());
			}
		} else if constexpr (std::is_same_v<T, ResourceCreateMenuRegistry::Branch>) {
			if (ImGui::BeginMenu(std::string(node.key).c_str())) {
				for (const ResourceCreateMenuRegistry::Node& child : arg) {
					DrawCraetePopupNode(child);
				}
				ImGui::EndMenu();
			}
		}
	}, node.data);
}

void ResourcesPanel::DrawCreatePopup() {
	auto& state = current_state;
	if (state.is_changed && !state.is_leaf && state.mouse_button == ImGuiMouseButton_Right) {
		ImGui::OpenPopup("Create Resource");
	}
	if (ImGui::BeginPopup("Create Resource")) {
		for (const ResourceCreateMenuRegistry::Node& child : ResourceCreateMenuRegistry::GetRootChildren()) {
			DrawCraetePopupNode(child);
		}
		ImGui::EndPopup();
	}
}

void ResourcesPanel::DrawUi() {
	ImGui::Begin("Resources");
	if (ImGui::Button("Import File")) {
		IGFD::FileDialogConfig config;
		config.path = ResourcesManager::GetInstance().GetAssetSourcesDirectory().string();
		config.countSelectionMax = 1;
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose Resource File", ".*", config);
	}
	TreeView::DrawUi(root, current_state);
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
