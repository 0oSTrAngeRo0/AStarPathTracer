#pragma once

#include "Utilities/Singleton.h"
#include "Engine/Resources/Resources.h"
#include <unordered_map>

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
};