#pragma once

#include "Core/Buffer.h"

class DeviceContext;

class ShaderBase {
protected:
	Buffer material_buffer;
	size_t material_count;
public:
	inline const vk::DeviceAddress GetBufferAddress() const { return material_buffer.GetDeviceAddress(); }
	void Destroy(const DeviceContext& context) { material_buffer.Destroy(context); }
};

class Material {
private:
	uint16_t index;
	ShaderBase& shader; // Todo: maybe use unique_ptr?
public:
	Material(uint16_t index, ShaderBase& shader) : index(index), shader(shader) {}
	template <typename TData> inline TData& GetData();
	inline const vk::DeviceAddress GetBufferAddress() const { return shader.GetBufferAddress(); }
	inline const uint16_t GetIndex() const { return index; }
};

template <typename TData>
class Shader : public ShaderBase {
private:
	std::vector<TData> materials;

public:
	// Todo: need to broadcast when buffer realloced
	void UpdateData(const DeviceContext& context) {
		if (materials.size() != material_count) {
			vk::BufferCreateInfo bci({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
			material_buffer = Buffer::CreateWithData<TData>(context, bci, materials);
			material_count = materials.size();
		}
		else {
			material_buffer.SetData<TData>(context, materials);
		}
	}

	inline Material CreateMaterial();
	inline TData& GetData(size_t index) {
		return materials[index];
	}
};

template<typename TData>
inline Material Shader<TData>::CreateMaterial() {
	uint16_t index = materials.size();
	materials.emplace_back(TData());
	return Material(index, *this);
}

template<typename TData>
inline TData& Material::GetData() {
	return static_cast<Shader<TData>&>(shader).GetData(index);
}
