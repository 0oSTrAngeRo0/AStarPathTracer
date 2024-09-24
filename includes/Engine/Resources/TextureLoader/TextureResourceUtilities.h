#pragma once

#include <cstddef>
#include "Engine/Resources/Resources.h"
#include "Engine/StaticRegistry.h"
#include "Utilities/MacroUtilities.h"

enum class TextureFormat {
	eSrgbR8G8B8A8
};

struct TextureData {
public:
	uint32_t width;
	uint32_t height;
	TextureFormat format;
	std::vector<std::byte> data;
};

using LoadTextureResourceFunction = std::function<TextureData(const ResourceBase&)>;
class LoadTextureResourceRegistry : public StaticRegistry<ResourceType, LoadTextureResourceFunction> {
public:
	template <typename TResource> static TextureData LoadTexture(const Resource<TResource>& resource);
};

#define REGISTER_LOAD_TEXTURE_RESOURCE(type) static bool ASTAR_UNIQUE_VARIABLE_NAME(register_load_texture_resource_) = (\
 (LoadTextureResourceRegistry::Register(Resource<type>::GetResourceTypeStatic(), \
	[](const ResourceBase& resource) { \
		return LoadTextureResourceRegistry::LoadTexture<type>(static_cast<const Resource<type>&>(resource)); \
	})) \
, true);