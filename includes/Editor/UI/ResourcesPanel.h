#pragma once

#include <memory>
#include <variant>
#include <functional>

#include "Editor/UI/TreeViewUtilities.h"
#include "Editor/UI/Inspectors/ResourceEditorRegistry.h"


class ResourcesPanel {
private:
	TreeView::State current_state;
	TreeView::Node root;
	void DrawCreatePopup();
	void DrawCraetePopupNode(const ResourceCreateMenuRegistry::Node& node);
public:
	ResourcesPanel();
	void DrawUi();
	inline const std::tuple<const bool, const std::string> GetCurrentSelection() const { 
		return std::tie(current_state.is_leaf, current_state.id);
	}
	inline const bool IsSelectionChanged() const { return current_state.is_changed; }
};