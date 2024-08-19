#include "Application/Materials.h"
#include <string>
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourceData.h"
#include "Engine/Resources/JsonSerializer.h"

template <> const std::string& Resource<MaterialResourceData<SimpleLitMaterialData>>::GetResourceTypeStatic() {
	static std::string type = "MaterialSimpleLit";
	return type;
}
JSON_SERIALIZER(SimpleLitMaterialData, <>, color);
REGISTER_RESOURCE_SERIALIZER(MaterialResourceData<SimpleLitMaterialData>);
REGISTER_RESOURCE_DESERIALIZER(MaterialResourceData<SimpleLitMaterialData>, ASTAR_DO_NOTHING);
REGISTER_GET_BASE_MATERIAL_DATA(MaterialResourceData<SimpleLitMaterialData>);

template <> const std::string& Resource<MaterialResourceData<LitghtMaterialData>>::GetResourceTypeStatic() {
	static std::string type = "MaterialLight";
	return type;
}
JSON_SERIALIZER(LitghtMaterialData, <>, color, intensity);
REGISTER_RESOURCE_SERIALIZER(MaterialResourceData<LitghtMaterialData>);
REGISTER_RESOURCE_DESERIALIZER(MaterialResourceData<LitghtMaterialData>, ASTAR_DO_NOTHING);
REGISTER_GET_BASE_MATERIAL_DATA(MaterialResourceData<LitghtMaterialData>);


#include "Editor/UI/Inspectors/ResourceInspector.h"
#include "Editor/UI/Inspectors/ResourceEditorRegistry.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

template <> void ResourceInspector<MaterialResourceData<SimpleLitMaterialData>>::Draw() {
	ImGuiColorEditFlags flags =
		ImGuiColorEditFlags_AlphaBar |
		ImGuiColorEditFlags_Float |
		ImGuiColorEditFlags_PickerHueWheel |
		ImGuiColorEditFlags_DisplayRGB;
	is_dirty |= ImGui::ColorEdit4("BaseColor", glm::value_ptr(data.resource_data.material_data.color), flags);
}
REGISTER_INSPECTOR_CREATOR(MaterialResourceData<SimpleLitMaterialData>);
REGISTER_RESOURCE_CREATE_MENU({ "Material" }, MaterialResourceData<SimpleLitMaterialData>);


template <> void ResourceInspector<MaterialResourceData<LitghtMaterialData>>::Draw() {
	ImGuiColorEditFlags flags =
		ImGuiColorEditFlags_AlphaBar |
		ImGuiColorEditFlags_Float |
		ImGuiColorEditFlags_PickerHueWheel |
		ImGuiColorEditFlags_DisplayRGB;
	is_dirty |= ImGui::ColorEdit3("BaseColor", glm::value_ptr(data.resource_data.material_data.color), flags);
}
REGISTER_INSPECTOR_CREATOR(MaterialResourceData<LitghtMaterialData>);
REGISTER_RESOURCE_CREATE_MENU({ "Material" }, MaterialResourceData<LitghtMaterialData>);