#include "Editor/UI/Inspectors/ResourceInspector.h"
#include "Engine/Resources/Resources.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

std::unordered_map<MaterialType, ResourceInspectorFactory::InspectorCreateFunction> ResourceInspectorFactory::material_resource_create_functions = {
	{
		MaterialType::eSimpleLit, [](ResourceBase& resource) {
			return std::make_unique<ResourceInspector<MaterialResourceData<SimpleLitMaterialData>>>(static_cast<ResourceTemplate<MaterialResourceData<SimpleLitMaterialData>>&>(resource));
		}
	}
};

std::unordered_map<ResourceType, ResourceInspectorFactory::InspectorCreateFunction> ResourceInspectorFactory::resource_create_functions = {
	{
		ResourceType::eObj, [](ResourceBase& resource) {
			return std::make_unique<ResourceInspector<ObjResourceData>>(static_cast<ResourceTemplate<ObjResourceData>&>(resource));
		}
	},
	{
		ResourceType::eMaterial, [](ResourceBase& resource) -> std::unique_ptr<EditorInspectorBase> {
			auto material_type = static_cast<ResourceTemplate<MaterialResourceDataBase>&>(resource).resource_data.material_type;
			auto iter = material_resource_create_functions.find(material_type);
			if (iter != material_resource_create_functions.end()) {
				return iter->second(resource);
			}
			else return std::make_unique<EmptyInspector>(); 
		}
	}
};

std::unique_ptr<EditorInspectorBase> ResourceInspectorFactory::CreateInspector(ResourceBase& resource) {
	auto iter = resource_create_functions.find(resource.resource_type);
	if (iter != resource_create_functions.end()) {
		return iter->second(resource);
	}
	else return std::make_unique<EmptyInspector>();
}

template <>
void ResourceInspector<ObjResourceData>::DrawInspector() {
	ImGui::LabelText("Source File Path", "%s", data.resource_data.path.c_str());
}

template <>
void ResourceInspector<MaterialResourceData<SimpleLitMaterialData>>::DrawInspector() {
	ImGuiColorEditFlags flags = 
		ImGuiColorEditFlags_AlphaBar | 
		ImGuiColorEditFlags_Float | 
		ImGuiColorEditFlags_PickerHueWheel | 
		ImGuiColorEditFlags_DisplayRGB;
	ImGui::ColorEdit4("BaseColor", glm::value_ptr(data.resource_data.material_data.color), flags);
}
