#pragma once

#include "Utilities/Singleton.h"
#include "Engine/Resources/Resources.h"
#include <unordered_map>
#include <memory>

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
	template <typename TData> ResourceBase& CreateNewResource(const std::string& path) {
		std::unique_ptr<ResourceBase> resource = std::make_unique<Resource<TData>>();
		Uuid uuid = resource->uuid;
		resources.insert_or_assign(uuid, ResourceData(path, std::move(resource)));
		SaveResource(uuid);
		return *resources.at(uuid).data;
	}
private:
	struct ResourceData {
		std::string path;
		std::unique_ptr<ResourceBase> data;

		ResourceData(const std::string& path, std::unique_ptr<ResourceBase> data) : path(path), data(std::move(data)) {}
	};
	std::unordered_map<Uuid, ResourceData> resources;
};
