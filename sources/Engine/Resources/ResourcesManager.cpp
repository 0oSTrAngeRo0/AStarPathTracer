#include "Engine/Resources/ResourcesManager.h"
#include "fstream"

ResourceBase& ResourcesManager::LoadResource(const std::string& path) {
	std::ifstream file(path);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file.");
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();

	std::unique_ptr<ResourceBase> resource = ResourceBase::Deserialize(content);
	Uuid uuid = resource->uuid;
	resources[uuid] = std::move(resource);
	return *resources[uuid];
}
