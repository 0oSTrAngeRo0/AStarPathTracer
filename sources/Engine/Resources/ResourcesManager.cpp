#include "Engine/Resources/ResourcesManager.h"
#include "fstream"
#include <filesystem>
#include <cassert>

std::vector<std::byte> ResourcesManager::LoadBinaryFile(const std::string& path) {
	std::ifstream file;
	if (!path.starts_with(base_directory.string())) {
		std::string full_path = base_directory.string() + path;
		file = std::ifstream(full_path, std::ios::ate | std::ios::binary);
	}
	else file = std::ifstream(path, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to load file!");
	}
	size_t file_size = file.tellg();
	std::vector<char> buffer(file_size);
	file.seekg(0);
	file.read(buffer.data(), file_size);
	file.close();
	return reinterpret_cast<std::vector<std::byte>&>(buffer);
}

ResourcesManager::ResourcesManager() {
	ChangeBaseDirectory("D:/C++/Projects/PathTracer/EngineRuntime/");

	const std::filesystem::path& directory = GetResourcesDirectory();
	assert(std::filesystem::exists(directory) && std::filesystem::is_directory(directory));

	// 遍历目录中的所有项
	for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
		if (std::filesystem::is_regular_file(entry)) {
			const std::string& path = entry.path().string();
			std::printf("Loading resource: [%s]\n", path.c_str());
			LoadResource(path);
		}
	}
}

ResourceBase& ResourcesManager::LoadResource(const std::string& path) {
	std::ifstream file;
	if (!path.starts_with(base_directory.string())) {
		std::string full_path = base_directory.string() + path;
		file = std::ifstream(full_path);
	}
	else file = std::ifstream(path);

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

const ResourceBase& ResourcesManager::GetResource(const Uuid& id) const {
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
