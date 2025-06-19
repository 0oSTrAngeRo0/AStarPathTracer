#pragma once

#include <entt/entt.hpp>
#include "Editor/UI/TreeViewUtilities.h"

class HierachiesPanel {
public:
	void DrawUi(const entt::registry& registry);
	inline bool IsSelectionChanged() const { return current_state.is_changed; }
	inline const std::tuple<const bool, const std::string> GetCurrentSelection() const { 
		return std::make_tuple(current_state.is_leaf, current_state.id); 
	}
private:
	TreeView::Result last_result;
	bool is_selection_changed;
};