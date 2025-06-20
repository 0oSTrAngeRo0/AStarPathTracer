#pragma once

#include <string>
#include <memory>
#include <entt/entt.hpp>

class EditorInspectorBase;

class EditorSelection {
private:
	std::shared_ptr<EditorInspectorBase> selected_inspector;
public:
	EditorSelection();
	void SelectResource(const std::string& path);
	void SelectEntity(entt::entity entity, entt::registry& registry);
	void DrawUi();
};