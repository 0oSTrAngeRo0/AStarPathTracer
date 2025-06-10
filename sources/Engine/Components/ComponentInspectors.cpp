#include "Engine/Components/Camera.h"
#include "Engine/Components/Name.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Render.h"
#include <glm/gtc/type_ptr.hpp>
#include "Editor/UI/ImGuiX.h"
#include "Editor/UI/Inspectors/ComponentInspector.h"
#include "Utilities/MacroUtilities.h"
#include "Utilities/ReflectionX.h"
#include "Utilities/EnumX.h"
#include "entt/meta/factory.hpp"
#include <misc/cpp/imgui_stdlib.h>

template <typename TComponent>
static void RegisterMeta(std::string name) {
	reflection::RegisterTypeName<TComponent>(name);
	reflection::RegisterComponentInspector<TComponent, ComponentInspector<TComponent>>();
}

static void RegisterMeta() {
	RegisterMeta<Camera>("Camera");
	RegisterMeta<ProjectionCamera>("ProjectionCamera");
	RegisterMeta<OrbitCamera>("OrbitCamera");
	RegisterMeta<Name>("Name");
	RegisterMeta<LocalPosition>("LocalPosition");
	RegisterMeta<LocalRotation>("LocalRotation");
	RegisterMeta<LocalScale>("LocalScale");
	RegisterMeta<LocalTransform>("LocalTransform");
	RegisterMeta<LocalLinearVelocity>("LocalLinearVelocity");
	RegisterMeta<MaterialComponent>("Material");
	RegisterMeta<MeshComponent>("Mesh");
}

ASTAR_BEFORE_MAIN(RegisterMeta());

template <> void ComponentInspector<Camera>::DrawInspector() {
	ImGui::InputFloatMatrix("Projection", glm::value_ptr(component.projection), 4, 4, "%.3f", ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloatMatrix("View", glm::value_ptr(component.view), 4, 4, "%.3f", ImGuiInputTextFlags_ReadOnly);
}

template <> void ComponentInspector<ProjectionCamera>::DrawInspector() {
	ImGui::InputFloat("Near Z", &component.near_z);
	ImGui::InputFloat("Far Z", &component.far_z);
	ImGui::InputFloat("Fov Y Degree", &component.fov_y_degree);
	ImGui::InputFloat("Aspect", &component.aspect);
}

template <> void ComponentInspector<OrbitCamera>::DrawInspector() {
	ImGui::InputScalar("Look At", ImGuiDataType_U32, &component.look_at, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("Distance", &component.distance);
	ImGui::InputFloat("Theta", &component.theta);
	ImGui::InputFloat("Phi", &component.phi);
}

template <> void ComponentInspector<Name>::DrawInspector() {
	ImGui::InputText(CatId("Name").c_str(), &component.name);
}

template <> void ComponentInspector<LocalPosition>::DrawInspector() {
	ImGui::InputFloat3("Position", glm::value_ptr(component.position));
}

template <> void ComponentInspector<LocalRotation>::DrawInspector() {
	ImGui::InputFloat4("Rotation", glm::value_ptr(component.rotation));
}

template <> void ComponentInspector<LocalScale>::DrawInspector() {
	ImGui::InputFloat3("Scale", glm::value_ptr(component.scale));
}

template <> void ComponentInspector<LocalTransform>::DrawInspector() {
	ImGui::InputFloatMatrix("Transform", glm::value_ptr(component.matrix), 4, 4, "%.3f", ImGuiInputTextFlags_ReadOnly);
}

template <> void ComponentInspector<LocalLinearVelocity>::DrawInspector() {
	ImGui::InputFloat3("Velocity", glm::value_ptr(component.velocity));
}

template <> void ComponentInspector<MaterialComponent>::DrawInspector() {
	ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_DefaultOpen;
	if (!ImGui::TreeNodeEx("Material Resources", tree_node_flags)) return;

	for (size_t i = 0, end = component.resource_ids.size(); i < end; i++) {
		std::string id_str = component.resource_ids[i].str();
		ImGui::InputText(std::to_string(i).c_str(), &id_str, ImGuiInputTextFlags_ReadOnly);
	}
}

template <> void ComponentInspector<MeshComponent>::DrawInspector() {
	std::string id_str = component.resource_id.str();
	ImGui::InputText("Mesh Resource", &id_str, ImGuiInputTextFlags_ReadOnly);
	size_t submesh_count = component.device_ids.size();
	ImGui::InputScalar("Submesh Count", ImGuiDataType_U64, &submesh_count, nullptr, nullptr, "%u", ImGuiInputTextFlags_ReadOnly);
}
