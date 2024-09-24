#pragma once

#include "Engine/Guid.h"
#include "Core/Image.h"

class DeviceContext;

class TexturePool {
public:
	static constexpr uint32_t InvalidHandle = std::numeric_limits<uint32_t>::max();

	TexturePool() : is_dirty(false) {}
	inline std::vector<vk::DescriptorImageInfo> GetDescriptorData() const { return texture_array; }
	inline bool IsDirty() const { return is_dirty; }
	inline void SetDirty(bool dirty) { is_dirty = dirty; }
	uint32_t GetOrAddHandle(const Uuid& id);
	void Arrange(std::vector<Uuid> used_textures);
	void Ensure(const DeviceContext& context);
	void Destroy(const DeviceContext& context);
private:
	struct TextureData {
		bool is_loaded;
		Image image;
		uint32_t index;
	};
	
	std::vector<vk::DescriptorImageInfo> texture_array;
	std::unordered_map<Uuid, TextureData> texture_map;
	bool is_dirty;
};