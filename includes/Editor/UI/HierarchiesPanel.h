#pragma once

#include <entt/entt.hpp>
#include "Editor/UI/TreeViewUtilities.h"

class HierarchiesPanel {
public:
	void Refresh(const entt::registry& registry);
	TreeView::Result DrawUi(const entt::registry& registry, const TreeView::NodeId& selected);
	inline bool IsSelectionChanged() const { return is_selection_changed; }
	inline const std::tuple<const bool, const std::string> GetCurrentSelection() const { 
		if (!last_result.clicked) {
			return std::make_tuple(false, "");
		}
		auto& clicked = last_result.clicked.value().get();
		return std::make_tuple(clicked.is_leaf, clicked.id); 
	}
private:
	TreeView::Result last_result;
	bool is_selection_changed;
	TreeView::NodeId current_selection;
	TreeView::Node root;

	void DrawPanelMenuContext(const entt::registry& registry);
};
