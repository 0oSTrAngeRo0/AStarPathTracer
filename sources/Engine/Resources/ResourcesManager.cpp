#include "Engine/Resources/ResourcesManager.h"
#include "fstream"
#include <filesystem>
#include <cassert>

ResourcesManager::ResourcesManager() {
	assert(std::filesystem::exists(RESOURCES_DIR) && std::filesystem::is_directory(RESOURCES_DIR));

	// 遍历目录中的所有项
	for (const auto& entry : std::filesystem::recursive_directory_iterator(RESOURCES_DIR)) {
		if (std::filesystem::is_regular_file(entry)) {
			LoadResource(entry.path().string());
		}
	}
}

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

ResourceBase& ResourcesManager::GetResource(const Uuid& id) {
	if (!resources.contains(id)) {
		throw std::runtime_error("Failed to find resources");
	}
	return *resources.at(id).data;
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
