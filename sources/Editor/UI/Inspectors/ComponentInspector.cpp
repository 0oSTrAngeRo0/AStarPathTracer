#include "Editor/UI/Inspectors/ComponentInspector.h"
#include <imgui.h>

EntityInspector::EntityInspector(entt::entity entity, entt::registry& registry) {
	for (auto [id, storage] : registry.storage()) {
		if (!storage.contains(entity)) continue;
		auto& type_info = storage.type();
		auto inspector_opt = reflection::CreateComponentInspector(type_info, registry, entity);
		if (inspector_opt) components.emplace_back(std::move(inspector_opt.value()));
		else components.emplace_back(std::make_unique<UnknownComponentInspector>(type_info));
	}
}

void EntityInspector::DrawInspector() {
	for (auto& component : components) {
		if (!ImGui::CollapsingHeader(component->GetComponentName().c_str())) continue;
		component->DrawInspector();
	}
}

EntityInspector::~EntityInspector() {}

UnknownComponentInspector::UnknownComponentInspector(const entt::type_info& type_info) :
	name(reflection::GetTypeName(type_info.hash()).value_or(std::string(type_info.name()))) {}

void UnknownComponentInspector::DrawInspector() {
	ImGui::Text("Unimplemented inspector!");
}
