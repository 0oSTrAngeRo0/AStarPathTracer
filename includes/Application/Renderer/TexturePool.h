#pragma once

#include "Engine/Guid.h"
#include "Core/Image.h"

class DeviceContext;

class TexturePool {
public:
	static constexpr uint32_t InvalidHandle = std::numeric_limits<uint32_t>::max();
	std::vector<vk::DescriptorImageInfo> GetDescriptorData() const;
	inline uint32_t GetHandle(const Uuid& id) const { 
		auto it = texture_map.find(id);
		return (it != texture_map.end()) ? it->second : InvalidHandle;
	}
	void Arrange(std::vector<Uuid> used_textures);
	void Ensure();
private:
	struct TextureData {
		bool is_loaded;
		vk::Image image;
		vk::ImageView image_view;
		vk::Sampler sampler;
		vk::ImageLayout layout;
	};
	std::vector<TextureData> texture_array;
	std::unordered_map<Uuid, uint32_t> texture_map;
};