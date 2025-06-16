#pragma once

#include "Engine/StaticRegistry.h"
#include "Utilities/MacroUtilities.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Editor/UI/Inspectors/ResourceInspector.h"

class EditorInspectorBase;

class ResourceInspectorCreateRegistry : public StaticFunctionRegistry<ResourceTypeDisplay, std::unique_ptr<EditorInspectorBase>(ResourceBase&)> {
public:
	using Base = StaticFunctionRegistry<ResourceTypeDisplay, std::unique_ptr<EditorInspectorBase>(ResourceBase&)>;
	template <typename T> static void Register() {
		Base::Register(Resource<T>::type_display,
		[](ResourceBase& resource) {
			return std::make_unique<ResourceInspector<T>>(static_cast<Resource<T>&>(resource));
		});
	}
};
class ResourceCreateMenuRegistry : public StaticTreeRegistry<ResourceTypeDisplay, std::function<void(const std::string&)>> {
public:
	using Base = StaticTreeRegistry<ResourceTypeDisplay, std::function<void(const std::string&)>>;
	template <typename T> static void Register(std::initializer_list<ResourceTypeDisplay> paths) {
		Base::AddLeaf([](const std::string& directory) {	
			std::string filename = directory + "/New " + std::string(Resource<T>::type_display) + ".json"; 
			ResourcesManager::GetInstance().CreateNewResource<T>(filename); 
		}, paths, Resource<T>::type_display);
	}
};
