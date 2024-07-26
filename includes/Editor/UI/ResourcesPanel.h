#pragma once

#include <memory>
#include <variant>
#include <functional>

#include "Editor/UI/FileBrowser.h"


class ResourcesPanel {
private:
	struct CreateResourceMenuItemNode {
	public:
		std::string label;
		std::variant<std::function<void()>, std::vector<CreateResourceMenuItemNode>> data;
	};
	static std::vector<CreateResourceMenuItemNode> creaet_resource_data;

	std::unique_ptr<FileBrowser> browser;
	void DrawCreatePopup();
	void DrawCraetePopupNode(const CreateResourceMenuItemNode& node);
public:
	ResourcesPanel();
	void DrawUi();
	inline const std::tuple<const bool, const std::string> GetCurrentSelection() const { 
		auto& current = browser->GetCurrentState();
		return std::tie(current.is_directory, current.full_path);
	}
	inline const bool IsSelectionChanged() const { return browser->GetCurrentState().is_changed; }
};