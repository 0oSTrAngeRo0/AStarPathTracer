#include <imgui.h>
#include "Editor/EditorSelection.h"
#include "Editor/UI/Inspectors/EditorInspector.h"
#include "Editor/UI/Inspectors/ResourceInspector.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Editor/UI/Inspectors/ComponentInspector.h"


EditorSelection::EditorSelection() {
	selected_inspector = std::make_shared<EmptyInspector>();
}

void EditorSelection::SelectResource(const std::string& path) {
	ResourceBase& resource = ResourcesManager::GetInstance().LoadResource(path);
	selected_inspector = std::move(ResourceInspectorFactory::CreateInspector(resource));
}

void EditorSelection::SelectEntity(entt::entity entity, entt::registry& registry) {
	selected_inspector = std::make_shared<EntityInspector>(entity, registry);
}

void EditorSelection::DrawUi() {
	ImGui::Begin("Inspector");
	if (selected_inspector) {
		selected_inspector->DrawInspector();
	}
	ImGui::End();
}
