#include <imgui.h>
#include "Editor/UI/HierarchiesPanel.h"
#include "Engine/Components/Name.h"
#include "Utilities/EnumX.h"
#include "Utilities/MacroUtilities.h"
#include "Engine/Components/JsonSerialize.h"

void HierarchiesPanel::Refresh(const entt::registry& registry) {
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
	this->root = root;
}

TreeView::Result HierarchiesPanel::DrawUi(const entt::registry& registry, const TreeView::NodeId& selected) {
	Refresh(registry);

	ImGui::Begin("Hierarchies");
	auto result = TreeView::DrawUiNoRoot(root, selected);
	if (result.clicked && result.mouse_button == ImGuiMouseButton_Right) {
		auto& clicked = result.clicked.value().get();
		ImGui::OpenPopup("HierarchiesContextWithItem");
	}
	if (ImGui::BeginPopup("HierarchiesContextWithItem")) {
		if (ImGui::MenuItem("Print Item")) {
			ASTAR_PRINT("On EnTT::Entity Print\n");
		}
		ImGui::Separator();
		DrawPanelMenuContext(registry);
		ImGui::EndPopup();
	}
	else if (ImGui::BeginPopupContextWindow("HierarchiesContext", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight)) {
		DrawPanelMenuContext(registry);
		ImGui::EndPopup();
	}
	ImGui::End();

	return result;
}

void HierarchiesPanel::DrawPanelMenuContext(const entt::registry& registry) {
	if (ImGui::MenuItem("Print Registry")) {
		nlohmann::json j = registry;
		ASTAR_PRINT("Registry: \n%s\n", j.dump(4).c_str());
	}
}
