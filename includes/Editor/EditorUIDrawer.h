#pragma once

#include <entt/entt.hpp>
#include <memory>

class EditorSelection;
class ResourcesPanel;
class HierachiesPanel;
class ViewportPanel;

class EditorUIDrawer {
private:
	std::unique_ptr<ResourcesPanel> resources_panel;
	std::unique_ptr<HierachiesPanel> hierachies_panel;
	std::unique_ptr<EditorSelection> selection;
	std::unique_ptr<ViewportPanel> viewport_panel;
public:
	EditorUIDrawer();
	void DrawUI(entt::registry& registry);
	~EditorUIDrawer();
private:
	void DrawInspetor(entt::registry& registry);
};