#include <imgui.h>
#include "Editor/UI/HierarchiesPanel.h"
#include "Engine/Components/Name.h"
#include "Utilities/EnumX.h"

void HierachiesPanel::DrawUi(const entt::registry& registry) {
	auto view = registry.view<Name>();
	TreeView::Node root;
	root.is_leaf = false;
	root.id = "Root";
	root.name = "Root";
	view.each([&root](const entt::entity entity, const Name& name) {
		TreeView::Node leaf;
		leaf.id = std::to_string(GetEnumUnderlying(entity));
		leaf.name = name;
		leaf.is_leaf = true;
		root.children.emplace_back(leaf);
	});

	ImGui::Begin("Hierachies");
	TreeView::DrawUiNoRoot(root, current_state);
	ImGui::End();
}
