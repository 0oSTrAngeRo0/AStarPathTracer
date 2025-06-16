#pragma once

#include "Engine/StaticRegistry.h"
#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include "Utilities/MacroUtilities.h"
#include "Engine/Resources/Resources.h"

class ResourceSerializeRegistry : public StaticFunctionRegistry<std::string, void(nlohmann::json&, const ResourceBase&)> {
public:
	using Base = StaticFunctionRegistry<std::string, void(nlohmann::json&, const ResourceBase&)>;
	template <typename T>
	static void Register() {
		Base::Register(std::string(Resource<T>::type_display), [](nlohmann::json& j, const ResourceBase& obj) {
			j = static_cast<const Resource<T>&>(obj);
		});
	}
};

class ResourceDeserializerRegistry : public StaticFunctionRegistry<std::string, std::optional<std::unique_ptr<ResourceBase>>(const nlohmann::json&)> {
public:
	using TReturn = std::optional<std::unique_ptr<ResourceBase>>;
	using Base = StaticFunctionRegistry<std::string, TReturn(const nlohmann::json&)>;
	template <typename T>
	static void Register() {
		Base::Register(std::string(Resource<T>::type_display), 
		[](const nlohmann::json& j) -> TReturn { 
			auto ptr = std::make_unique<Resource<T>>(j.template get<Resource<T>>()); 
			return ptr; 
		});
	}

	template <typename T>
	static void RegisterWithAfter(std::function<void(const Resource<T>&)> callback) {
		Base::Register(std::string(Resource<T>::type_display), 
		[callback](const nlohmann::json& j) -> TReturn {
			auto ptr = std::make_unique<Resource<T>>(j.template get<Resource<T>>()); 
			callback((*ptr)); 
			return ptr; 
		});
	}
};
