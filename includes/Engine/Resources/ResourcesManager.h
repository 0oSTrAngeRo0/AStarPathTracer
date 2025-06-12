#pragma once

#include "Engine/Resources/Resources.h"
#include "Utilities/Singleton.h"
#include <memory>
#include <string_view>
#include <unordered_map>
#include <filesystem>

class ResourcesManager : public Singleton<ResourcesManager> {
  public:
	ResourcesManager();
	ResourceBase& LoadResource(const std::string& path);
	ResourceBase& GetResource(const Uuid& id);
	const ResourceBase& GetResource(const Uuid& id) const;
	void SaveResource(const Uuid& uuid);
	void IterateResources(std::function<void(const std::string&, const ResourceBase&)> callback) {
		for (const auto& pair : resources) {
			callback(pair.second.path, *pair.second.data);
		}
	}
	template <typename TData>
	ResourceBase& CreateNewResource(const std::string& path) {
		std::unique_ptr<ResourceBase> resource = std::make_unique<Resource<TData>>();
		Uuid uuid = resource->uuid;
		resources.insert_or_assign(uuid, ResourceData(path, std::move(resource)));
		SaveResource(uuid);
		return *resources.at(uuid).data;
	}
	inline const std::filesystem::path& GetAssetSourcesDirectory() const { return asset_sources_directory; }
	inline const std::filesystem::path& GetResourcesDirectory() const { return resources_directory; }
	inline void ChangeBaseDirectory(std::string path) {
		if(!path.ends_with('/') && path.ends_with("\\\\"))
		base_directory = path;
		asset_sources_directory = path + "AssetSources/";
		resources_directory = path + "Resources/";
	}

  private:
	struct ResourceData {
		std::string path;
		std::unique_ptr<ResourceBase> data;

		ResourceData(const std::string& path, std::unique_ptr<ResourceBase> data)
			: path(path), data(std::move(data)) {}
	};
	std::unordered_map<Uuid, ResourceData> resources;
	std::filesystem::path base_directory;
	std::filesystem::path asset_sources_directory;
	std::filesystem::path resources_directory;
};
