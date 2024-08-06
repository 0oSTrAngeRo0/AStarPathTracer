#include "Application/Renderer/MeshPool.h"
#include "acceleration-structure.h"
#include "Engine/Resources/MeshLoader/MeshResourceUtilities.h"
#include <algorithm>

const Mesh& MeshPool::GetMesh(const Uuid& id) {
	if (!meshes.contains(id)) {
		throw std::runtime_error("Invalid mesh id" + id.str());
	}
	return meshes.at(id);
}

void MeshPool::EnsureMeshes(const DeviceContext& context, const std::vector<std::tuple<Uuid, Uuid>>& used_meshes) {
	std::vector<std::reference_wrapper<Mesh>> uninitialized_meshes;
	for (const auto& pair : used_meshes) {
		const auto& id = std::get<0>(pair);
		if (meshes.contains(id)) {
			continue;
		}
		const auto& resource = std::get<1>(pair);
		const auto& data = MeshResourceUtilities::Load(resource);
		Mesh mesh(context, data.positions, data.normals, data.tangents, data.uvs, data.indices);
		meshes.insert(std::make_pair(id, mesh));
		uninitialized_meshes.emplace_back(meshes.at(id));
	}
	if (uninitialized_meshes.size() == 0) return;
	CreateBlases(context, uninitialized_meshes);
	UpdateBuffers(context);
}

void MeshPool::ReleaseUnusedMeshes(const DeviceContext& context, std::vector<Uuid> used) {
	bool is_dirty = false;
	std::erase_if(meshes, [&used, &context, &is_dirty](const auto& pair) {
		if (std::find(used.begin(), used.end(), pair.first) == used.end()) {
			Mesh mesh = pair.second;
			mesh.Destroy(context);
			is_dirty = true;
			return true;
		}
		return false;
	});
	if (is_dirty) {
		UpdateBuffers(context);
	}
}

void MeshPool::Destroy(const DeviceContext& context) {
	ReleaseUnusedMeshes(context, {});
	vertex_buffer.Destroy(context);
	index_buffer.Destroy(context);
}

void MeshPool::UpdateBuffers(const DeviceContext& context) {
	if (meshes.size() == 0) {
		index_buffer.Destroy(context);
		vertex_buffer.Destroy(context);
		return;
	}
	std::vector<vk::DeviceAddress> vertex;
	std::vector<vk::DeviceAddress> index;
	for (const auto& pair : meshes) {
		vertex.emplace_back(pair.second.GetVertexAddress());
		index.emplace_back(pair.second.GetIndexAddress());
	}

	const uint32_t batch_size = 64;
	vk::BufferCreateInfo create_info({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
	vma::AllocationCreateInfo allocation_info({}, vma::MemoryUsage::eAuto);
	if (Buffer::SetDataWithResize<vk::DeviceAddress>(context, index_buffer, index, batch_size, create_info, allocation_info)) {
		index_buffer.SetName(context, "Index Buffer");
	}
	if (Buffer::SetDataWithResize<vk::DeviceAddress>(context, vertex_buffer, vertex, batch_size, create_info, allocation_info)) {
		vertex_buffer.SetName(context, "Vertex Buffer");
	}
}
