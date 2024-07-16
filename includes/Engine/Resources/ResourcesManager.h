#pragma once

#include "Utilities/Singleton.h"
#include "Engine/Resources/Resources.h"
#include <unordered_map>

class ResourcesManager : public Singleton<ResourcesManager> {
private:
	std::unordered_map<Uuid, std::unique_ptr<ResourceBase>> resources;
public:
	ResourceBase& LoadResource(const std::string& path);
};