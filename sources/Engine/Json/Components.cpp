#include "Engine/Json/Components.h"
#include "Engine/Components/JsonSerialize.h"

template <typename T>
inline static void RegisterJson() {
	reflection::RegisterJsonSerialize<T>();
	reflection::RegisterJsonDeserialize<T>();
}

static void RegisterComponentJsonMeta() {
	RegisterJson<Camera>();
	RegisterJson<ProjectionCamera>();
	RegisterJson<OrbitCamera>();
	RegisterJson<Name>();
	RegisterJson<LocalPosition>();
	RegisterJson<LocalRotation>();
	RegisterJson<LocalScale>();
	RegisterJson<LocalTransform>();
	RegisterJson<LocalLinearVelocity>();
	RegisterJson<MaterialComponent>();
	RegisterJson<MeshComponent>();
}

ASTAR_BEFORE_MAIN(RegisterComponentJsonMeta());