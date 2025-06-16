#include <string>
#include <cstddef>
#include "Utilities/MacroUtilities.h"
#include "Engine/StaticRegistry.h"
#include "Engine/Resources/Resources.h"

struct TextureResourceData {
public:
	std::string path;
};
template<> constexpr ResourceTypeDisplay Resource<TextureResourceData>::type_display = "Texture" ;

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

class StbImageUtilities {
public:
	static TextureData Load(const std::string& path);
};

class LoadTextureResourceRegistry : public StaticFunctionRegistry<ResourceTypeId, TextureData(const ResourceBase&)> {
public:
	using Base = StaticFunctionRegistry<ResourceTypeId, TextureData(const ResourceBase&)>;
	template <typename TResource> static TextureData LoadTexture(const Resource<TResource>& resource);
	template <typename T> static void Register() {
		Base::Register(Resource<T>::type_id, 
		[](const ResourceBase& resource) { 
			return LoadTextureResourceRegistry::LoadTexture<T>(static_cast<const Resource<T>&>(resource)); 
		});
	}
};
