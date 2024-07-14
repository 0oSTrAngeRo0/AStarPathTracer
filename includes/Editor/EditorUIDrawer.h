#pragma once


#include <entt/entt.hpp>
#include <memory>
#include "Editor/UI/FileBrowser.h"


class EditorUIDrawer {
private:
	std::unique_ptr<FileBrowser> assets_browser;
public:
	EditorUIDrawer();
	void DrawUI(entt::registry& registry);
};