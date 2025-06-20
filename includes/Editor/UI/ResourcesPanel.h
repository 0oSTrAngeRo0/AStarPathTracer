#pragma once

#include <memory>
#include <variant>
#include <functional>

#include "Editor/UI/TreeViewUtilities.h"
#include "Editor/UI/Inspectors/ResourceEditorRegistry.h"


class ResourcesPanel {
private:
	bool is_selection_changed;
	TreeView::Result last_result;
	TreeView::Node root;
	void DrawCreatePopup();
	void DrawCraetePopupNode(const ResourceCreateMenuRegistry::Node& node);
public:
	ResourcesPanel();
	TreeView::Result DrawUi(const TreeView::NodeId& selected);
	inline const std::tuple<const bool, const std::string> GetCurrentSelection() const { 
		if (!last_result.clicked) {
			return std::tie<const bool, const std::string>(false, root.id);
		}
		auto result = last_result.clicked.value().get();
		return std::tie(result.is_leaf, result.id);
	}
	inline const bool IsSelectionChanged() const { return is_selection_changed; }
};