#include <string>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include "Application/Materials.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Engine/Resources/ResourceData.h"
#include "Engine/Resources/ResourceRegistry.h"
#include "Utilities/JsonX.h"
#include "Engine/Json/Glm.h"
#include "Engine/Json/Guid.h"
#include "Engine/Json/Resource.h"
#include "Application/Renderer/RenderContext.h"
#include "Editor/UI/Inspectors/ResourceInspector.h"
#include "Editor/UI/Inspectors/ResourceEditorRegistry.h"

#define GET_MATERIAL_TYPE(type) ASTAR_CONCAT(type, MaterialData)
#define GET_MATERIAL_RESOURCE_TYPE(type) MaterialResourceData<type##MaterialData>
#define GET_MATERIAL_RUNTIME_TYPE(type) ASTAR_CONCAT(type, MaterialRuntimeData)

#define UPDATE_MATERIAL_DATA_FUNC(type, resource, context) \
template <> \
GET_MATERIAL_RUNTIME_TYPE(type) UpdateMaterialDataRegistry::UpdateMaterialData( \
	const Resource<GET_MATERIAL_RESOURCE_TYPE(type)>& resource, \
	const MaterialVisiableContext& context \
)

#define MATERIAL_JSON_SERIALIZER(type, ...) JSON_SERIALIZER(GET_MATERIAL_TYPE(type), <>, __VA_ARGS__) 


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
MATERIAL_JSON_SERIALIZER(SimpleLit, diffuse_color, diffuse_texture);


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
MATERIAL_JSON_SERIALIZER(Light, color, intensity);


// Pure Reflection Material Implementation
template <> void ResourceInspector<MaterialResourceData<PureReflectionMaterialData>>::Draw() {}
using GET_MATERIAL_RUNTIME_TYPE(PureReflection) = GET_MATERIAL_TYPE(PureReflection);
UPDATE_MATERIAL_DATA_FUNC(PureReflection, resource, context) {
	return resource.resource_data.material_data;
}
MATERIAL_JSON_SERIALIZER(PureReflection, color);


// Dielectric Material Implementation
template <> void ResourceInspector<MaterialResourceData<DielectricMaterialData>>::Draw() {
	is_dirty |= ImGui::InputFloat("Eta", &data.resource_data.material_data.eta);
}
using GET_MATERIAL_RUNTIME_TYPE(Dielectric) = GET_MATERIAL_TYPE(Dielectric);
UPDATE_MATERIAL_DATA_FUNC(Dielectric, resource, context) {
	return resource.resource_data.material_data;
}
MATERIAL_JSON_SERIALIZER(Dielectric, eta);


template <typename T, typename TRuntime>
static void Register() {
	using TMaterial = MaterialResourceData<T>;
	ResourceSerializeRegistry::Register<TMaterial>();
	ResourceDeserializerRegistry::Register<TMaterial>();
	GetMaterialBaseFunctionRegistry::Register<TMaterial>();
	ResourceInspectorCreateRegistry::Register<TMaterial>();
	ResourceCreateMenuRegistry::Register<TMaterial>({ "Material" });
	UpdateMaterialDataRegistry::Register<TMaterial, TRuntime>();
}

static void Register() {
	Register<SimpleLitMaterialData, SimpleLitMaterialRuntimeData>();
	Register<LightMaterialData, LightMaterialRuntimeData>();
	Register<PureReflectionMaterialData, PureReflectionMaterialRuntimeData>();
	Register<DielectricMaterialData, DielectricMaterialRuntimeData>();
}
ASTAR_BEFORE_MAIN(Register());
