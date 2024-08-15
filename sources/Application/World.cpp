#include "Application/World.h"
#include "Engine/InputSystem.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Render.h"
#include "Application/Renderer/RenderContext.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Engine/ShaderHostBuffer.h"
#include "Engine/HostShaderManager.h"

#pragma region Systems

void World::UpdateMaterialsRegistry(entt::registry& registry) {
	auto view = registry.view<const MaterialComponent>();
	view.each([&](const MaterialComponent& material) {
		const auto& [shader_id, index] = HostShaderManager::GetInstance().RegisterMaterial(material.resource_id);
	});
}

void World::UpdateLinearVelocity(entt::registry& registry, float delta_time) {
	auto view = registry.view<LocalLinearVelocity, LocalPosition>();
	view.each([delta_time](const LocalLinearVelocity& velocity, LocalPosition& position) {
		position.position += velocity.velocity * delta_time;
		});
}

void World::UpdateLocalTransform(entt::registry& registry) {
	auto view = registry.view<
		const LocalPosition,
		const LocalRotation,
		const LocalScale,
		LocalTransform
	>();

	view.each([](
		const LocalPosition& position,
		const LocalRotation& rotation,
		const LocalScale& scale,
		LocalTransform& transform
		) {
			glm::mat4 mat(1.0);
			mat = glm::translate(mat, position.position);
			mat *= glm::mat4_cast(rotation.rotation);
			mat = glm::scale(mat, scale.scale);
			transform.matrix = mat;
		});
}

void World::UpdateOrbitCamera(entt::registry& registry) {
	const InputState& input = registry.ctx().get<const InputState&>();
	if (input.GetMouseButton(InputState::MouseButton::eRight) == InputState::ActionState::eRelease) return;
	glm::vec2 delta = input.GetMousePositionDelta();
	float scrollY = input.GetMouseScroll().y;
	auto view = registry.view<LocalPosition, LocalRotation, OrbitCamera>();
	view.each([&delta, &scrollY, &registry](LocalPosition& position, LocalRotation& rotation, OrbitCamera& orbit) {
		// Todo: fix filp on 90 degree
		orbit.theta += delta.x;
		orbit.phi += delta.y;
		orbit.distance -= scrollY;

		float x = orbit.distance * sin(glm::radians(orbit.phi)) * cos(glm::radians(orbit.theta));
		float y = orbit.distance * cos(glm::radians(orbit.phi));
		float z = orbit.distance * sin(glm::radians(orbit.phi)) * sin(glm::radians(orbit.theta));

		glm::vec3 look_at = registry.get<LocalPosition>(orbit.look_at).position;
		glm::mat4x4 matrix = glm::inverse(glm::lookAt(glm::vec3(x, y, z), look_at, glm::vec3(0, 1, 0)));
		position.position = matrix[3];
		rotation.rotation = glm::quat_cast(matrix);
		});
}

void World::UpdateProjectiveCamera(entt::registry& registry) {
	auto view = registry.view<const LocalTransform, const ProjectionCamera, Camera>();
	view.each([](const LocalTransform& transform, const ProjectionCamera& data, Camera& camera) {
		camera.projection = glm::perspective(glm::radians(data.fov_y), data.aspect, data.near_z, data.far_z);
		camera.view = glm::inverse(transform.matrix);
		});
}

#pragma endregion

#pragma region World Creations

entt::entity World::CreateCamera(entt::registry& registry) {
	entt::entity e = registry.create();
	registry.emplace<LocalPosition>(e, glm::vec3(0, 0, -3));
	registry.emplace<LocalRotation>(e, glm::quatLookAt(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)));
	registry.emplace<LocalScale>(e, glm::vec3(1, 1, 1));
	registry.emplace<LocalTransform>(e);
	registry.emplace<ProjectionCamera>(e, 45, 1, 0.1, 100);
	registry.emplace<Camera>(e);
	return e;
}

entt::entity World::CreateCube(entt::registry& registry) {
	entt::entity e = registry.create();
	registry.emplace<LocalPosition>(e, glm::vec3(0, 0, 0));
	registry.emplace<LocalRotation>(e, glm::quatLookAt(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)));
	registry.emplace<LocalScale>(e, glm::vec3(1, 1, 1));
	registry.emplace<LocalTransform>(e);

	registry.emplace<MeshComponent>(e, Uuid("c98fb2af-8be5-437b-b096-bc44d71b656d"));
	registry.emplace<MaterialComponent>(e, Uuid("5ba3b79d-2f7e-479a-961b-04b58ba8fc6c"));
	return e;
}

void World::CreateDefault(entt::registry& registry) {
	entt::entity camera = CreateCamera(registry);
	entt::entity cube = CreateCube(registry);
	registry.emplace<OrbitCamera>(camera, cube, 3, 0, 0);
}

#pragma endregion

void World::Update(entt::registry& registry, float delta_time) {
	UpdateMaterialsRegistry(registry);
	UpdateOrbitCamera(registry);
	UpdateLinearVelocity(registry, delta_time);
	UpdateLocalTransform(registry);
	UpdateProjectiveCamera(registry);
}
