#pragma once

#include "Utilities/Singleton.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourceData.h"
#include <unordered_map>
#include <memory>

class ResourcesManager : public Singleton<ResourcesManager> {
private:
	struct ResourceData {
		std::string path;
		std::unique_ptr<ResourceBase> data;

		ResourceData(const std::string& path, std::unique_ptr<ResourceBase> data) : path(path), data(std::move(data)){}
	};
	std::unordered_map<Uuid, ResourceData> resources;
public:
	ResourceBase& LoadResource(const std::string& path);
	void SaveResource(const Uuid& uuid);
	template <typename TData> ResourceBase& CreateNewResource(const std::string& path);
};

template<typename TData>
inline ResourceBase& ResourcesManager::CreateNewResource(const std::string& path) {
	std::unique_ptr<ResourceBase> resource = std::make_unique<Resource<TData>>();
	Uuid uuid = resource->uuid;
	resources.insert_or_assign(uuid, ResourceData(path, std::move(resource)));
	SaveResource(uuid);
	return *resources.at(uuid).data;
}
