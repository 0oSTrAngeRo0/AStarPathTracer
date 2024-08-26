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
	registry.view<const OrbitCamera, const CameraDirtyTag>().each([&registry](entt::entity entity, const OrbitCamera& orbit) {
		registry.remove<CameraDirtyTag>(entity);
		});
	const InputState& input = registry.ctx().get<const InputState&>();
	if (input.GetMouseButton(InputState::MouseButton::eRight) == InputState::ActionState::eRelease) return;
	glm::vec2 delta = input.GetMousePositionDelta();
	float scroll_y = input.GetMouseScroll().y;
	//std::printf("Mouse Delta: [(%f, %f)]; Mouse Scroll: [%f]\n", delta.x, delta.y, scroll_y);
	auto view = registry.view<LocalPosition, LocalRotation, OrbitCamera>();
	view.each([&delta, &scroll_y, &registry](entt::entity entity, LocalPosition& position, LocalRotation& rotation, OrbitCamera& orbit) {
		// Todo: fix filp on 90 degree
		orbit.theta += delta.x;
		orbit.phi += delta.y;
		orbit.distance -= scroll_y;

		float x = orbit.distance * sin(glm::radians(orbit.phi)) * cos(glm::radians(orbit.theta));
		float y = orbit.distance * cos(glm::radians(orbit.phi));
		float z = orbit.distance * sin(glm::radians(orbit.phi)) * sin(glm::radians(orbit.theta));

		glm::vec3 look_at = registry.get<LocalPosition>(orbit.look_at).position;
		glm::mat4x4 matrix = glm::inverse(glm::lookAt(glm::vec3(x, y, z), look_at, glm::vec3(0, 1, 0)));
		position.position = matrix[3];
		rotation.rotation = glm::quat_cast(matrix);

		registry.emplace_or_replace<CameraDirtyTag>(entity);
		});
}

void World::UpdateProjectiveCamera(entt::registry& registry) {
	auto view = registry.view<const LocalTransform, const ProjectionCamera, Camera>();
	view.each([](const LocalTransform& transform, const ProjectionCamera& data, Camera& camera) {
		glm::mat4x4 projection = glm::perspective(glm::radians(data.fov_y), data.aspect, data.near_z, data.far_z);
		projection[1][1] *= -1;
		camera.projection = projection;
		camera.view = glm::inverse(transform.matrix);
		});
}

#pragma endregion

#pragma region World Creations

entt::entity World::CreateCamera(entt::registry& registry) {
	entt::entity e = registry.create();
	registry.emplace<LocalPosition>(e, glm::vec3(0, 0, 3));
	registry.emplace<LocalRotation>(e, glm::quatLookAt(glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)));
	registry.emplace<LocalScale>(e, glm::vec3(1, 1, 1));
	registry.emplace<LocalTransform>(e);
	registry.emplace<ProjectionCamera>(e, 45, 1, 0.1, 100);
	registry.emplace<Camera>(e);
	return e;
}

entt::entity World::CreateCube(entt::registry& registry) {
	entt::entity e = registry.create();
	registry.emplace<LocalPosition>(e, glm::vec3(0, 0, 0));
	registry.emplace<LocalRotation>(e, glm::quat(1, 0, 0, 0));
	registry.emplace<LocalScale>(e, glm::vec3(1, 1, 1));
	registry.emplace<LocalTransform>(e);

	registry.emplace<MeshComponent>(e, Uuid("c98fb2af-8be5-437b-b096-bc44d71b656d"));
	registry.emplace<MaterialComponent>(e, Uuid("5ba3b79d-2f7e-479a-961b-04b58ba8fc6c"));
	return e;
}

entt::entity World::CreateLight(entt::registry& registry) {
	entt::entity e = registry.create();
	registry.emplace<LocalPosition>(e, glm::vec3(0, 2, 0));
	registry.emplace<LocalRotation>(e, glm::quat(1, 0, 0, 0));
	registry.emplace<LocalScale>(e, glm::vec3(1, 1, 1));
	registry.emplace<LocalTransform>(e);

	registry.emplace<MeshComponent>(e, Uuid("0ba11610-6979-4284-a533-10a61c70363d"));
	registry.emplace<MaterialComponent>(e, Uuid("99116883-fd22-4e1c-a6f2-d5ccc1b94a3d"));
	return e;
}

void World::CreateCornellBox(entt::registry& registry) {
}

void World::CreateAxises(entt::registry& registry, float length, float size) {
	Uuid materials[] = { 
		Uuid("283e6726-58d0-4ffb-a92f-7b0599bca3d9"), 
		Uuid("7c39efcd-3c1e-4735-b390-799a2b7cf736"), 
		Uuid("aae8b14d-42cd-4ad5-8bd8-b7cbb19dac64") 
	};
	glm::vec3 scales[] = {
		glm::vec3(length, size, size),
		glm::vec3(size, length, size),
		glm::vec3(size, size, length),
	};
	float offset = length * 0.5;
	glm::vec3 positions[] = {
		glm::vec3(offset, 0, 0),
		glm::vec3(0, offset, 0),
		glm::vec3(0, 0, offset),
	};
	for (size_t i = 0; i < 3; i++) {
		entt::entity e = registry.create();
		registry.emplace<LocalPosition>(e, positions[i]);
		registry.emplace<LocalRotation>(e, glm::quat(1, 0, 0, 0));
		registry.emplace<LocalScale>(e, scales[i]);
		registry.emplace<LocalTransform>(e);

		registry.emplace<MeshComponent>(e, Uuid("c98fb2af-8be5-437b-b096-bc44d71b656d"));
		registry.emplace<MaterialComponent>(e, materials[i]);
	}
}

void World::CreateDefault(entt::registry& registry) {
	CreateAxises(registry, 2, 0.1);
	entt::entity camera = CreateCamera(registry);
	entt::entity cube = CreateCube(registry);
	entt::entity light = CreateLight(registry);
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
