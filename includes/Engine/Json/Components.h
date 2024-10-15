#pragma once 

#include "Utilities/JsonX.h"
#include "Engine/Json/Glm.h"
#include "Engine/Json/Guid.h"

#include "Engine/Components/Name.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Render.h"

JSON_SERIALIZER(Name, <>, name);
JSON_SERIALIZER(ProjectionCamera, <>, near_z, far_z, aspect, fov_y_degree);
JSON_SERIALIZER(OrbitCamera, <>, look_at, distance, theta, phi);
JSON_SERIALIZER(Camera, <>, view, projection);
JSON_SERIALIZER(LocalLinearVelocity, <>, velocity);
JSON_SERIALIZER(LocalPosition, <>, position);
JSON_SERIALIZER(LocalRotation, <>, rotation);
JSON_SERIALIZER(LocalScale, <>, scale);
JSON_SERIALIZER(LocalTransform, <>, matrix);
JSON_SERIALIZER(MeshComponent, <>, resource_id, device_ids);
JSON_SERIALIZER(MaterialComponent, <>, resource_ids);
