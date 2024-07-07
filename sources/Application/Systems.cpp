#include "Application/Systems.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Render.h"
#include "Core/RenderContext.h"

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

entt::entity CreateCube(entt::registry& registry, RenderContext& renderer) {
	entt::entity e = registry.create();
	registry.emplace<LocalPosition>(e, glm::vec3(0, 0, 0));
	registry.emplace<LocalRotation>(e, glm::quatLookAt(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)));
	registry.emplace<LocalScale>(e, glm::vec3(1, 1, 1));
	registry.emplace<LocalTransform>(e);
	registry.emplace<LocalLinearVelocity>(e, glm::vec3(0, 0.01, 0));

	auto& render = registry.emplace<Renderable>(e);
	render.mesh = renderer.meshes[0];

	// setup material
	render.material = renderer.shaders[0]->CreateMaterial();
	std::shared_ptr<MaterialTemplate<LitMaterialData>> material = std::static_pointer_cast<MaterialTemplate<LitMaterialData>>(render.material);
	printf("Material Address [%d]\n\n", (int)material->GetIndex());
	LitMaterialData material_data = material->GetData();
	material_data.color = glm::vec4(0.3, 0.3, 0.7, 1.0);
	material->SetData(material_data);

	// setup instance
	std::shared_ptr<InstanceHandler> instance = renderer.instances.CreateInstance();
	render.instance = instance;
	return e;
}

Systems::Systems(RenderContext& renderer) {
	CreateCamera(registry);
	CreateCube(registry, renderer);
}

void Systems::Update(float time) {
	UpdateLinearVelocity(registry, time);
	UpdateLocalTransform(registry);
	UpdateProjectiveCamera(registry);
}

Systems::~Systems() {}
