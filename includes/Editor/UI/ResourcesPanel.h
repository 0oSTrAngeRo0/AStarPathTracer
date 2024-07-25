#pragma once

#include <memory>

#include "Editor/UI/FileBrowser.h"


class ResourcesPanel {
private:
	std::unique_ptr<FileBrowser> browser;
public:
	ResourcesPanel();
	void DrawUi();
	inline const std::tuple<const bool, const std::string> GetCurrentSelection() const { 
		auto current = browser->GetCurrentSelection();
		return std::tie(current.is_directory, current.full_path);
	}
	inline const bool IsSelectionChanged() const { return browser->IsSelectionChanged(); }
};