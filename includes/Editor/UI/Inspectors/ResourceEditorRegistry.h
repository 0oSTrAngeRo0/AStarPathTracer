#pragma once

#include "Engine/StaticRegistry.h"

class EditorInspectorBase;
class ResourceBase;

class ResourceInspectorCreateRegistry : public StaticRegistry<std::string, std::function<std::unique_ptr<EditorInspectorBase>(ResourceBase&)>> {};
class ResourceCreateMenuRegistry : public StaticTreeRegistry<std::string, std::function<void(const std::string&)>> {};

#define REGISTER_INSPECTOR_CREATOR(name, type) \
static bool resource_inspector_register_##name = (ResourceInspectorCreateRegistry::Register(#name, \
	[](ResourceBase& resource) { \
		return std::make_unique<ResourceInspector<type>>(static_cast<Resource<type>&>(resource)); \
	}), true)

#define REGISTER_RESOURCE_CREATE_MENU(name, path, key, type) \
static bool resource_create_menu_register_##name = (ResourceCreateMenuRegistry::AddLeaf( \
	[](const std::string& directory) {	\
		std::string filename = directory + "/New " + #name + ".json"; \
		ResourcesManager::GetInstance().CreateNewResource<type>(filename); \
	}, path, key), true)