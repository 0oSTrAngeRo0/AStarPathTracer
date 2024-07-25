#include "Editor/EditorSelection.h"
#include "Editor/UI/Inspectors/EditorInspector.h"
#include "Editor/UI/Inspectors/ResourceInspector.h"
#include "Engine/Resources/ResourcesManager.h"
#include <fstream>

EditorSelection::EditorSelection() {
	selected_inspector = std::make_shared<EmptyInspector>();
}

void EditorSelection::SelectResource(const std::string& path) {
	ResourceBase& resource = ResourcesManager::GetInstance().LoadResource(path);
	selected_inspector = std::move(ResourceInspectorFactory::CreateInspector(resource));
}
