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
	void Refresh(std::string root_path);
	void DrawCreatePopup();
	void DrawCraetePopupNode(const ResourceCreateMenuRegistry::Node& node);
public:
	ResourcesPanel();
	void DrawUi();
	inline const std::tuple<const bool, const std::string> GetCurrentSelection() const { 
		return std::tie(current_state.is_not_leaf, current_state.full_path);
	}
	inline const bool IsSelectionChanged() const { return current_state.is_changed; }
};