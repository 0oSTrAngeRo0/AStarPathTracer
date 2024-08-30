#include "Engine/Resources/MeshLoader/MeshResourceUtilities.h"
#include "Engine/Resources/ResourcesManager.h"

std::vector<MeshData> MeshResourceUtilities::Load(Uuid id) {
	auto& resource = ResourcesManager::GetInstance().GetResource(id);
	auto& type = resource.GetResourceType();

	auto function_opt = MeshResourceUtilities::Get(resource.GetResourceType());
	if (function_opt.has_value()) {
		return function_opt.value()(resource);
	}
	throw std::runtime_error("Invalid resource type" + type);
}
