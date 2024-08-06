#include "Application/Systems.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Render.h"
#include "Application/Renderer/RenderContext.h"

void UpdateLinearVelocity(entt::registry& registry, float delta) {
	auto view = registry.view<LocalLinearVelocity, LocalPosition>();
	view.each([delta](const LocalLinearVelocity& velocity, LocalPosition& position) {
		position.position += velocity.velocity * delta;
	});
}

void UpdateLocalTransform(entt::registry& registry) {
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

void UpdateProjectiveCamera(entt::registry& registry) {
	auto view = registry.view<const LocalTransform, const ProjectionCamera, Camera>();
	view.each([](const LocalTransform& transform, const ProjectionCamera& data, Camera& camera) {
		camera.projection = glm::perspective(glm::radians(data.fov_y), data.aspect, data.near_z, data.far_z);
		camera.view = glm::inverse(transform.matrix);
	});
}

entt::entity CreateCamera(entt::registry& registry) {
	entt::entity e = registry.create();
	registry.emplace<LocalPosition>(e, glm::vec3(0, 0, -3));
	registry.emplace<LocalRotation>(e, glm::quatLookAt(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)));
	registry.emplace<LocalScale>(e, glm::vec3(1, 1, 1));
	registry.emplace<LocalTransform>(e);
	registry.emplace<ProjectionCamera>(e, ProjectionCamera(45, 1, 0.1, 100));
	registry.emplace<Camera>(e);
	return e;
}

entt::entity CreateCube(entt::registry& registry) {
	entt::entity e = registry.create();
	registry.emplace<LocalPosition>(e, glm::vec3(0, 0, 0));
	registry.emplace<LocalRotation>(e, glm::quatLookAt(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)));
	registry.emplace<LocalScale>(e, glm::vec3(1, 1, 1));
	registry.emplace<LocalTransform>(e);
	registry.emplace<LocalLinearVelocity>(e, glm::vec3(0, 0.01, 0));

	registry.emplace<MeshComponent>(e, Uuid("c98fb2af-8be5-437b-b096-bc44d71b656d"));
	registry.emplace<MaterialComponent>(e);
	return e;
}

Systems::Systems() {
	CreateCamera(registry);
	CreateCube(registry);
}

void Systems::Update(float time) {
	UpdateLinearVelocity(registry, time);
	UpdateLocalTransform(registry);
	UpdateProjectiveCamera(registry);
}

Systems::~Systems() {
	entt::snapshot snapshot(registry);
}
