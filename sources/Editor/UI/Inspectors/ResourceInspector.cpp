#include "Editor/UI/Inspectors/ResourceInspector.h"
#include "Engine/Resources/Resources.h"
#include <imgui.h>

std::unique_ptr<EditorInspectorBase> ResourceInspectorFactory::CreateInspector(ResourceBase& resource) {
	if (resource.resource_type == ResourceType::eObj) {
		return std::make_unique<ObjResourceInspector>(static_cast<ResourceTemplate<ObjResourceData>&>(resource));
	}
	return std::unique_ptr<EmptyInspector>();
}

void ObjResourceInspector::DrawInspector() {
	//ImGui::BeginTable("")
	ImGui::LabelText("Source File Path", "%s", data.resource_data.path.c_str());
}
