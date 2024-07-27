#pragma once

#include <memory>
#include <variant>
#include <functional>

#include "Editor/UI/FileBrowser.h"
#include "Editor/UI/Inspectors/ResourceEditorRegistry.h"


class ResourcesPanel {
private:
	std::unique_ptr<FileBrowser> browser;
	void DrawCreatePopup();
	void DrawCraetePopupNode(const ResourceCreateMenuRegistry::Node& node);
public:
	ResourcesPanel();
	void DrawUi();
	inline const std::tuple<const bool, const std::string> GetCurrentSelection() const { 
		auto& current = browser->GetCurrentState();
		return std::tie(current.is_directory, current.full_path);
	}
	inline const bool IsSelectionChanged() const { return browser->GetCurrentState().is_changed; }
};