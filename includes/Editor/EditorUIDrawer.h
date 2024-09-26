#pragma once

#include <entt/entt.hpp>
#include <memory>

class EditorSelection;
class ResourcesPanel;
class HierachiesPanel;

class EditorUIDrawer {
private:
	std::unique_ptr<ResourcesPanel> resources_panel;
	std::unique_ptr<HierachiesPanel> hierachies_panel;
	std::unique_ptr<EditorSelection> selection;
public:
	EditorUIDrawer();
	void DrawUI(entt::registry& registry);
	~EditorUIDrawer();
};