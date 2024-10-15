#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Engine/Guid.h"
#include <vulkan/vulkan.hpp>
#include "Engine/ShaderHostBuffer.h"
#include "Engine/StaticRegistry.h"
#include "Engine/Resources/Resources.h"
#include "Utilities/JsonX.h"

struct ObjResourceData {
public:
	std::string path; // relative path
	// other import settings
};

struct ShaderResourceData {
	std::string source_code_path;
	std::string compiled_code_path;
	vk::ShaderStageFlagBits shader_stage;
	uint32_t instance_stride;
	std::string entry_function;
};

struct MaterialResourceDataBase {
public:
	Uuid shader_id;

	virtual size_t RegisterToShader(const Uuid& self_id, HostShader& shader) const = 0;
};

class GetMaterialBaseFunctionRegistry : public StaticRegistry<std::string, std::function<const MaterialResourceDataBase& (const ResourceBase&)>> {};

#define REGISTER_GET_BASE_MATERIAL_DATA(type) \
static bool ASTAR_UNIQUE_VARIABLE_NAME(get_base_material_register_) = (GetMaterialBaseFunctionRegistry::Register(Resource<type>::GetResourceTypeStatic(), \
	[](const ResourceBase& resource) -> const MaterialResourceDataBase& { \
		return static_cast<const MaterialResourceDataBase&>(static_cast<const Resource<type>&>(resource).resource_data); \
	}), true); \


template <typename T>
struct MaterialResourceData : MaterialResourceDataBase {
public:
	T material_data;
	size_t RegisterToShader(const Uuid& self_id, HostShader& shader) const override {
		return shader.EmplaceValue<T>(self_id, material_data);
	}
};

JSON_SERIALIZER(MaterialResourceData<TMatData>, <typename TMatData>, material_data, shader_id);


struct TextureResourceData {
public:
	std::string path;
};

