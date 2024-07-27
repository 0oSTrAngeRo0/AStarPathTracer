#pragma once

#include "Engine/StaticRegistry.h"

class EditorInspectorBase;

class ResourceInspectorCreateRegistry : public StaticRegistry<std::string, std::function<std::unique_ptr<EditorInspectorBase>(ResourceBase&)>> {};

#define REGISTER_INSPECTOR_CREATOR(name, type) \
static bool resource_inspector_register_##name = (ResourceInspectorCreateRegistry::Register(#name, \
	[](ResourceBase& resource) { \
		return std::make_unique<ResourceInspector<type>>(static_cast<Resource<type>&>(resource)); \
	}), true)
