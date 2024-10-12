#include "Editor/UI/Inspectors/ComponentInspector.h"
#include <imgui.h>

void EntityInspector::DrawInspector() {
	bool need_refresh = false;
	std::string entity_str = std::to_string(GetEnumUnderlying(entity));

	for (auto& component : components) {
		const std::string& name = component.inspector->GetComponentName();
		if (ImGui::CollapsingHeader(name.c_str()))
			component.inspector->DrawInspector();

		std::string popup_id = entity_str + name;
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			ImGui::OpenPopup(popup_id.c_str());
		if (ImGui::BeginPopup(popup_id.c_str())) {
			if (ImGui::MenuItem("Remove Compnent")) {
				registry.storage(component.type)->remove(entity);
				need_refresh = true;
			}
			ImGui::EndPopup();
		}
	}

	std::string add_component_id = entity_str + "AddComponent";
	if (ImGui::Button("Add Component")) {
		ImGui::OpenPopup(add_component_id.c_str());
	}
	if (ImGui::BeginPopup(add_component_id.c_str())) {
		for (auto& component : other_components) {
			if (ImGui::MenuItem(component.name.c_str())) {
				registry.storage(component.type)->push(entity);
				need_refresh = true;
			}
		}
		ImGui::EndPopup();
	}

	if (need_refresh)
		Refresh();
}

EntityInspector::~EntityInspector() {}

void EntityInspector::Refresh() {
	other_components.clear();
	components.clear();
	for (auto [id, storage] : registry.storage()) {
		auto& type_info = storage.type();
		if (storage.contains(entity)) {
			auto inspector_opt = reflection::CreateComponentInspector(type_info, registry, entity);
			if (inspector_opt)
				components.emplace_back(ComponentData(id, std::move(inspector_opt.value())));
			else
				components.emplace_back(ComponentData(id, std::make_unique<UnknownComponentInspector>(type_info)));
		}
		else {
			std::string name = reflection::GetTypeName(id).value_or(std::string(type_info.name()));
			other_components.emplace_back(OtherComponentData(id, std::move(name)));
		}
	}
}

UnknownComponentInspector::UnknownComponentInspector(const entt::type_info& type_info) :
	name() {}

void UnknownComponentInspector::DrawInspector() {
	ImGui::Text("Unimplemented inspector!");
}
