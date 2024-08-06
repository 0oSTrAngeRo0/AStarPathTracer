#include "Engine/Resources/MeshLoader/MeshResourceUtilities.h"
#include "Engine/Resources/ResourcesManager.h"

MeshData MeshResourceUtilities::Load(Uuid id) {
	auto& resource = ResourcesManager::GetInstance().GetResource(id);
	auto& type = resource.resource_type;

	auto function_opt = MeshResourceUtilities::Get(resource.resource_type);
	if (function_opt.has_value()) {
		return function_opt.value()(resource);
	}
	throw std::runtime_error("Invalid resource type" + type);
}
