#include "Engine/Resources/ResourcesManager.h"
#include "fstream"

ResourceBase& ResourcesManager::LoadResource(const std::string& path) {
	std::ifstream file(path);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file.");
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	std::string content = buffer.str();

	std::unique_ptr<ResourceBase> resource = ResourceBase::Deserialize(content);
	Uuid uuid = resource->uuid;
	resources.insert_or_assign(uuid, ResourceData(path, std::move(resource)));
	return *resources.at(uuid).data;
}

void ResourcesManager::SaveResource(const Uuid& uuid) {
	if (!resources.contains(uuid)) {
		throw std::runtime_error(std::format("Invalid uuid:{0}", uuid.str()));
	}
	const ResourceData& data = resources.at(uuid);
	std::string json = ResourceBase::Serialize(*data.data);

	std::ofstream file(data.path);
	if (!file.is_open()) {
		throw std::runtime_error(std::format("Failed to open file: [{0}]", data.path));
	}

	file << json;
	file.close();
}
