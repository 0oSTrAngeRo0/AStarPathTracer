#include "Utilities/MacroUtilities.h"

#define STBI_ASSERT(x) ASTAR_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include "Engine/Resources/TextureLoader/stb_image.h"

#include "Engine/Resources/TextureLoader/StbImageUtilities.h"
#include "Engine/Resources/TextureLoader/TextureResourceUtilities.h"
#include "Engine/Resources/ResourceData.h"

TextureData StbImageUtilities::Load(const std::string& path) {
	// load data
	int width; int height; int channels;
	stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

	// copy to vector
	static_assert(sizeof(stbi_uc) % sizeof(std::byte) == 0);
	constexpr size_t size_coff = STBI_rgb_alpha * sizeof(stbi_uc) / sizeof(std::byte);
	std::byte* pixels_bytes = reinterpret_cast<std::byte*>(pixels);
	size_t size = width * height * size_coff;
	std::vector<std::byte> data(pixels_bytes, pixels_bytes + size);

	// free cache
	stbi_image_free(pixels);

	// fill return value
	TextureData texture;
	texture.data = data;
	texture.width = width;
	texture.height = height;
	texture.format = TextureFormat::eSrgbR8G8B8A8;
	return texture;
}

template <>
TextureData LoadTextureResourceRegistry::LoadTexture(const Resource<TextureResourceData>& resource) {
	return StbImageUtilities::Load(resource.resource_data.path);
}
REGISTER_LOAD_TEXTURE_RESOURCE(TextureResourceData)