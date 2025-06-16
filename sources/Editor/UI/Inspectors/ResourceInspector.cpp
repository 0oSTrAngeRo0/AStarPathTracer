#include "Editor/UI/Inspectors/ResourceInspector.h"
#include "Engine/Resources/Resources.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <Engine/Resources/ResourceData.h>
#include "Editor/UI/Inspectors/ResourceEditorRegistry.h"
#include "Engine/Resources/ObjResourceData.h"

std::unique_ptr<EditorInspectorBase> ResourceInspectorFactory::CreateInspector(ResourceBase& resource) {
	auto function = ResourceInspectorCreateRegistry::Get(resource.GetResourceTypeDisplay());
	if (function.has_value()) {
		return function.value()(resource);
	}
	return std::make_unique<EmptyInspector>();
}
