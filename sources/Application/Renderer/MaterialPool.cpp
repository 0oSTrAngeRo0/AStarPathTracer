#include "Application/Renderer/MaterialPool.h"
#include <unordered_set>
#include "Engine/Resources/ResourcesManager.h"

const Buffer& MaterialPool::GetShaderBuffer(const Uuid& id) {
	if (!shader_buffers.contains(id)) {
		throw std::runtime_error("Invalid shader id" + id.str());
	}
	return shader_buffers.at(id);
}

void MaterialPool::EnsureBuffers(const DeviceContext& context, const std::vector<std::tuple<const Uuid, const std::vector<std::byte>, const bool>>& material_data) {
	bool is_dirty = false;
	if (material_data.size() != shader_buffers.size()) {
		is_dirty = true;
	}

	vk::BufferCreateInfo create_info({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
	vma::AllocationCreateInfo allocation_info({}, vma::MemoryUsage::eAuto);

	std::unordered_set<Uuid> used_materials;
	for (const auto& data : material_data) {
		if (!std::get<2>(data)) {
			continue;
		}

		// extract data
		const Uuid& id = std::get<0>(data);
		auto bytes = std::get<1>(data);

		used_materials.insert(id);

		// resize buffer
		Buffer buffer = shader_buffers[id];
		if (Buffer::SetDataWithResize<std::byte>(context, buffer, bytes, 128, create_info, allocation_info)) {
			shader_buffers[id] = buffer;
			is_dirty = true;
		}
	}

	// release unused material
	std::erase_if(shader_buffers, [&used_materials, &context, &is_dirty](const auto& pair) {
		if (!used_materials.contains(pair.first)) {
			Buffer buffer = pair.second;
			buffer.Destroy(context);
			is_dirty = true;
			return true;
		}
		return false;
	});

	// update main buffer
	std::vector<vk::DeviceAddress> materials;
	for (const auto& shader : shader_buffers) {
		materials.emplace_back(shader.second.GetDeviceAddress());
	}
	if (is_dirty) {
		Buffer::SetDataWithResize<vk::DeviceAddress>(context, main_buffer, materials, 16, create_info, allocation_info);
	}
}

void MaterialPool::EnsureMaterials(const std::vector<Uuid>& materials) {
	for (const Uuid& id : materials) {
		ResourcesManager::GetInstance().GetResource(id);
	}
}

void MaterialPool::Destroy(const DeviceContext& context) {
	for (auto& buffer : shader_buffers) {
		buffer.second.Destroy(context);
	}
	shader_buffers.clear();
	main_buffer.Destroy(context);
}
