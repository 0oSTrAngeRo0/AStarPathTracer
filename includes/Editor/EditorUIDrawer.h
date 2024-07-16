#pragma once

#include <entt/entt.hpp>
#include <memory>
#include "Editor/EditorSelection.h"
#include "Editor/UI/ResourcesPanel.h"

class EditorUIDrawer {
private:
	std::unique_ptr<ResourcesPanel> resources_panel;
	std::unique_ptr<EditorSelection> selection;
public:
	EditorUIDrawer();
	void DrawUI(entt::registry& registry);
};