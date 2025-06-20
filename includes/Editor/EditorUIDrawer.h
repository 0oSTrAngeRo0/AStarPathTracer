#pragma once

#include <entt/entt.hpp>
#include <memory>
#include "Editor/UI/TreeViewUtilities.h"

class EditorSelection;
class ResourcesPanel;
class HierarchiesPanel;

class EditorUIDrawer {
private:
	enum class SelectionType {
		eNone,
		eResources,
		eHierachies,
	};

	std::unique_ptr<ResourcesPanel> resources_panel;
	std::unique_ptr<HierarchiesPanel> hierachies_panel;
	std::unique_ptr<EditorSelection> selection;
	SelectionType selection_type;
	TreeView::NodeId selection_id;
	bool is_selection_leaf;
public:
	EditorUIDrawer();
	void DrawUI(entt::registry& registry);
	~EditorUIDrawer();
private:
	bool TryChangeSelection(TreeView::Result result, SelectionType type);
	TreeView::NodeId TryGetSelectionID(SelectionType type) {
		if (selection_type == type) return selection_id;
		else return TreeView::InvalidNodeId;
	}
};