#pragma once

#include <memory>

#include "Editor/UI/FileBrowser.h"


class ResourcesPanel {
private:
	std::unique_ptr<FileBrowser> browser;
public:
	ResourcesPanel();
	void DrawUi();
};