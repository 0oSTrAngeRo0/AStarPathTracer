#include "Editor/UI/Inspectors/ResourceInspector.h"
#include "Engine/Resources/Resources.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <Engine/Resources/ResourceData.h>
#include "Editor/UI/Inspectors/ResourceEditorRegistry.h"

std::unique_ptr<EditorInspectorBase> ResourceInspectorFactory::CreateInspector(ResourceBase& resource) {
	auto function = ResourceInspectorCreateRegistry::Get(resource.GetResourceType());
	if (function.has_value()) {
		return function.value()(resource);
	}
	return std::make_unique<EmptyInspector>();
}

template <>
void ResourceInspector<ObjResourceData>::Draw() {
	ImGui::LabelText("Source File Path", "%s", data.resource_data.path.c_str());
}
REGISTER_INSPECTOR_CREATOR(ObjResourceData);

