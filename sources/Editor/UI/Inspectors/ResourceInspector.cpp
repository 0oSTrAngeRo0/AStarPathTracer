#include "Editor/UI/Inspectors/ResourceInspector.h"
#include "Engine/Resources/Resources.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <Engine/Resources/ResourceData.h>
#include "Editor/UI/Inspectors/ResourceEditorRegistry.h"

std::unique_ptr<EditorInspectorBase> ResourceInspectorFactory::CreateInspector(ResourceBase& resource) {
	auto function = ResourceInspectorCreateRegistry::Get(resource.resource_type);
	if (function.has_value()) {
		return function.value()(resource);
	}
	return std::make_unique<EmptyInspector>();
}

template <>
void ResourceInspector<ObjResourceData>::Draw() {
	ImGui::LabelText("Source File Path", "%s", data.resource_data.path.c_str());
}

template <>
void ResourceInspector<MaterialResourceData<SimpleLitMaterialData>>::Draw() {
	ImGuiColorEditFlags flags =
		ImGuiColorEditFlags_AlphaBar |
		ImGuiColorEditFlags_Float |
		ImGuiColorEditFlags_PickerHueWheel |
		ImGuiColorEditFlags_DisplayRGB;
	is_dirty |= ImGui::ColorEdit4("BaseColor", glm::value_ptr(data.resource_data.material_data.color), flags);
}

REGISTER_INSPECTOR_CREATOR(Obj, ObjResourceData);
REGISTER_INSPECTOR_CREATOR(MaterialSimpleLit, MaterialResourceData<SimpleLitMaterialData>);

REGISTER_RESOURCE_CREATE_MENU(MaterialSimpleLit, { "Material" }, "SimpleLit", MaterialResourceData<SimpleLitMaterialData>);
