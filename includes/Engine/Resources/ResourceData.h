#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Engine/Guid.h"
#include <vulkan/vulkan.hpp>
#include "Engine/ShaderHostBuffer.h"
#include "Engine/StaticRegistry.h"
#include "Engine/Resources/Resources.h"
#include "Utilities/JsonX.h"

struct ShaderResourceData {
	std::string source_code_path;
	std::string compiled_code_path;
	vk::ShaderStageFlagBits shader_stage;
	uint32_t instance_stride;
	std::string entry_function;
};
template<> constexpr std::string_view Resource<ShaderResourceData>::type_display = "Shader" ;

struct MaterialResourceDataBase {
public:
	Uuid shader_id;

	virtual size_t RegisterToShader(const Uuid& self_id, HostShader& shader) const = 0;
};

class GetMaterialBaseFunctionRegistry : public StaticFunctionRegistry<ResourceTypeId, const MaterialResourceDataBase& (const ResourceBase&)> {
public:
	using Base = StaticFunctionRegistry<ResourceTypeId, const MaterialResourceDataBase& (const ResourceBase&)>;
	template <typename T>
	static void Register() {
		Base::Register(Resource<T>::type_id, 
		[](const ResourceBase& resource) -> const MaterialResourceDataBase& { 
			return static_cast<const MaterialResourceDataBase&>(static_cast<const Resource<T>&>(resource).resource_data); 
		});
	}
};

template <typename T>
struct MaterialResourceData : MaterialResourceDataBase {
public:
	T material_data;
	size_t RegisterToShader(const Uuid& self_id, HostShader& shader) const override {
		return shader.EmplaceValue<T>(self_id, material_data);
	}
};

JSON_SERIALIZER(MaterialResourceData<TMatData>, <typename TMatData>, material_data, shader_id);




