#include <imgui.h>
#include "Editor/UI/HierarchiesPanel.h"
#include "Engine/Components/Name.h"
#include "Utilities/EnumX.h"
#include "Utilities/MacroUtilities.h"

void HierachiesPanel::DrawUi(const entt::registry& registry) {
	auto view = registry.view<Name>();
	TreeView::Node root;
	root.is_leaf = false;
	root.id = "Root";
	root.name = "Root";
	view.each([&root](const entt::entity entity, const Name& name) {
		TreeView::Node leaf;
		leaf.id = std::to_string(GetEnumUnderlying(entity));
		leaf.name = name.name;
		leaf.is_leaf = true;
		root.children.emplace_back(leaf);
	});

	ImGui::Begin("Hierachies");
	TreeView::DrawUiNoRoot(root, current_state);
	if (current_state.mouse_button == ImGuiMouseButton_Right) {
		if (ImGui::BeginPopupContextWindow("HierachiesContextWithItem", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight)) {
			if (ImGui::MenuItem("Print Item")) {
				ASTAR_PRINT("On EnTT::Entity Print");
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Print Registry")) {
				ASTAR_PRINT("On EnTT::Registry Print");
			}
			ImGui::EndPopup();
		}
	}
	if (ImGui::BeginPopupContextWindow("HierachiesContext", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight)) {
		if (ImGui::MenuItem("Print Registry")) {
			ASTAR_PRINT("On EnTT::Registry Print");
		}
		ImGui::EndPopup();
	}
	ImGui::End();
}
