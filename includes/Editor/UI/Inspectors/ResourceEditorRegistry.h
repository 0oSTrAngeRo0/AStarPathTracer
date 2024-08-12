#pragma once

#include "Engine/StaticRegistry.h"
#include "Utilities/MacroUtilities.h"

class EditorInspectorBase;
class ResourceBase;

class ResourceInspectorCreateRegistry : public StaticRegistry<std::string, std::function<std::unique_ptr<EditorInspectorBase>(ResourceBase&)>> {};
class ResourceCreateMenuRegistry : public StaticTreeRegistry<std::string, std::function<void(const std::string&)>> {};

#define REGISTER_INSPECTOR_CREATOR(type) \
static bool ASTAR_UNIQUE_VARIABLE_NAME(resource_inspector_register_) = (ResourceInspectorCreateRegistry::Register(Resource<type>::GetResourceTypeStatic(), \
	[](ResourceBase& resource) { \
		return std::make_unique<ResourceInspector<type>>(static_cast<Resource<type>&>(resource)); \
	}), true)

#define REGISTER_RESOURCE_CREATE_MENU(path, type) \
static bool ASTAR_UNIQUE_VARIABLE_NAME(resource_create_menu_register_) = (ResourceCreateMenuRegistry::AddLeaf( \
	[](const std::string& directory) {	\
		std::string filename = directory + "/New " + Resource<type>::GetResourceTypeStatic() + ".json"; \
		ResourcesManager::GetInstance().CreateNewResource<type>(filename); \
	}, path, Resource<type>::GetResourceTypeStatic()), true)