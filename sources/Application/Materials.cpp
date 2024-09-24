#include "Application/Materials.h"
#include <string>
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Engine/Resources/ResourceData.h"
#include "Engine/Resources/JsonSerializer.h"
#include "Application/Renderer/RenderContext.h"

#include "Editor/UI/Inspectors/ResourceInspector.h"
#include "Editor/UI/Inspectors/ResourceEditorRegistry.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#define GET_MATERIAL_TYPE(type) ASTAR_CONCAT(type, MaterialData)
#define GET_MATERIAL_RESOURCE_NAME(type) ASTAR_GET_STRING(ASTAR_CONCAT(Material, type))
#define GET_MATERIAL_RESOURCE_TYPE(type) ASTAR_CONCAT(ASTAR_CONCAT(MaterialResourceData<, type), MaterialData>)
#define GET_MATERIAL_RUNTIME_TYPE(type) ASTAR_CONCAT(type, MaterialRuntimeData)

#define REGISTER_RESOURCE_TYPE(type, name) \
template <> const ResourceType& Resource<type>::GetResourceTypeStatic() { \
	static ResourceType type_value = name; \
	return type_value; \
}

#define UPDATE_MATERIAL_DATA_FUNC(type, resource, context) \
template <> \
GET_MATERIAL_RUNTIME_TYPE(type) UpdateMaterialDataRegistry::UpdateMaterialData( \
	const Resource<GET_MATERIAL_RESOURCE_TYPE(type)>& resource, \
	const MaterialVisiableContext& context \
)

#define GET_MATERIAL_RAW_DATA_DEFAULT_FUNC(type) GET_MATERIAL_RAW_DATA_FUNC(GET_MATERIAL_TYPE(type), resource, context) { \
	return reinterpret_cast<std::vector<std::byte>>(resource.resource_data); \
}

#define REGISTER_MATERIAL(type, ...) \
	REGISTER_RESOURCE_TYPE(GET_MATERIAL_RESOURCE_TYPE(type), GET_MATERIAL_RESOURCE_NAME(type)) \
	JSON_SERIALIZER(GET_MATERIAL_TYPE(type), <>, __VA_ARGS__) \
	REGISTER_RESOURCE_SERIALIZER(GET_MATERIAL_RESOURCE_TYPE(type)) \
	REGISTER_RESOURCE_DESERIALIZER(GET_MATERIAL_RESOURCE_TYPE(type)) \
	REGISTER_GET_BASE_MATERIAL_DATA(GET_MATERIAL_RESOURCE_TYPE(type)) \
	REGISTER_INSPECTOR_CREATOR(GET_MATERIAL_RESOURCE_TYPE(type)) \
	REGISTER_UPDATE_MATERIAL_DATA(GET_MATERIAL_RESOURCE_TYPE(type), GET_MATERIAL_RUNTIME_TYPE(type)) \
	REGISTER_RESOURCE_CREATE_MENU({ "Material" }, GET_MATERIAL_RESOURCE_TYPE(type)) \


// Simple Lit Material Implementation
template <> void ResourceInspector<MaterialResourceData<SimpleLitMaterialData>>::Draw() {
	ImGuiColorEditFlags flags =
		ImGuiColorEditFlags_AlphaBar |
		ImGuiColorEditFlags_Float |
		ImGuiColorEditFlags_PickerHueWheel |
		ImGuiColorEditFlags_DisplayRGB;
	is_dirty |= ImGui::ColorEdit4("BaseColor", glm::value_ptr(data.resource_data.material_data.diffuse_color), flags);
}
UPDATE_MATERIAL_DATA_FUNC(SimpleLit, resource, context) {
	SimpleLitMaterialRuntimeData runtime;
	SimpleLitMaterialData serialize = resource.resource_data.material_data;
	runtime.diffuse_color = serialize.diffuse_color;
	runtime.diffuse_texture = context.textures.GetOrAddHandle(serialize.diffuse_texture);
	return runtime;
};
REGISTER_MATERIAL(SimpleLit, diffuse_color, diffuse_texture);


// Light Material Implementation
template <> void ResourceInspector<MaterialResourceData<LightMaterialData>>::Draw() {
	ImGuiColorEditFlags flags =
		ImGuiColorEditFlags_AlphaBar |
		ImGuiColorEditFlags_Float |
		ImGuiColorEditFlags_PickerHueWheel |
		ImGuiColorEditFlags_DisplayRGB;
	is_dirty |= ImGui::ColorEdit3("BaseColor", glm::value_ptr(data.resource_data.material_data.color), flags);
}
using GET_MATERIAL_RUNTIME_TYPE(Light) = GET_MATERIAL_TYPE(Light);
UPDATE_MATERIAL_DATA_FUNC(Light, resource, context) {
	return resource.resource_data.material_data;
}
REGISTER_MATERIAL(Light, color, intensity);


// Pure Reflection Material Implementation
template <> void ResourceInspector<MaterialResourceData<PureReflectionMaterialData>>::Draw() {}
using GET_MATERIAL_RUNTIME_TYPE(PureReflection) = GET_MATERIAL_TYPE(PureReflection);
UPDATE_MATERIAL_DATA_FUNC(PureReflection, resource, context) {
	return resource.resource_data.material_data;
}
REGISTER_MATERIAL(PureReflection, color);


// Dielectric Material Implementation
template <> void ResourceInspector<MaterialResourceData<DielectricMaterialData>>::Draw() {
	is_dirty |= ImGui::InputFloat("Eta", &data.resource_data.material_data.eta);
}
using GET_MATERIAL_RUNTIME_TYPE(Dielectric) = GET_MATERIAL_TYPE(Dielectric);
UPDATE_MATERIAL_DATA_FUNC(Dielectric, resource, context) {
	return resource.resource_data.material_data;
}
REGISTER_MATERIAL(Dielectric, eta);
