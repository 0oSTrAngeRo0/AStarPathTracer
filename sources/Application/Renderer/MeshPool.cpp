#include "Application/Renderer/MeshPool.h"
#include "acceleration-structure.h"
#include "Engine/Resources/MeshLoader/MeshResourceUtilities.h"
#include <algorithm>

const Mesh& MeshPool::GetMesh(const Uuid& device_id) {
	if (!meshes.contains(device_id)) {
		throw std::runtime_error("Invalid mesh id" + device_id.str());
	}
	return meshes.at(device_id);
}

void MeshPool::EnsureMeshes(const DeviceContext& context, const std::vector<UsedMesh>& used_meshes) {
	std::vector<std::reference_wrapper<Mesh>> uninitialized_meshes;
	for (const auto& pair : used_meshes) {
		const auto& device_ids = std::get<0>(pair);
		uint32_t unloaded_mesh_count = 0;
		for(const auto& id : device_ids) {
			if (!meshes.contains(id)) unloaded_mesh_count++;
		}
		if (unloaded_mesh_count == 0) continue;
		ASTAR_ASSERT(unloaded_mesh_count == device_ids.size()); // some submesh are uploaded but some are not uploaded, it's invalid

		const auto& resource = std::get<1>(pair);
		const auto& submeshes = MeshResourceUtilities::Load(resource);
		for (size_t i = 0, end = submeshes.size(); i < end; i++) {
			const auto& submesh = submeshes[i];
			const auto& id = device_ids[i];
			Mesh mesh(context, submesh.positions, submesh.normals, submesh.tangents, submesh.uvs, submesh.indices);
			mesh.SetName(context, resource.str());
			meshes.insert(std::make_pair(id, mesh));
			uninitialized_meshes.emplace_back(meshes.at(id));
		}
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
	vertex_position_buffer.Destroy(context);
	index_buffer.Destroy(context);
	vertex_other_buffer.Destroy(context);
}

void MeshPool::UpdateBuffers(const DeviceContext& context) {
	if (meshes.size() == 0) {
		index_buffer.Destroy(context);
		vertex_position_buffer.Destroy(context);
		vertex_other_buffer.Destroy(context);
		return;
	}
	std::vector<vk::DeviceAddress> vertex_position;
	std::vector<vk::DeviceAddress> index;
	std::vector<vk::DeviceAddress> vertex_other;
	for (const auto& pair : meshes) {
		vertex_position.emplace_back(pair.second.GetVertexPositionAddress());
		vertex_other.emplace_back(pair.second.GetVertexOtherAddress());
		index.emplace_back(pair.second.GetIndexAddress());
	}

	const uint32_t batch_size = 64;
	vk::BufferCreateInfo create_info({}, {}, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
	vma::AllocationCreateInfo allocation_info({}, vma::MemoryUsage::eAuto);
	if (Buffer::SetDataWithResize<vk::DeviceAddress>(context, index_buffer, index, batch_size, create_info, allocation_info)) {
		index_buffer.SetName(context, "Index Buffer");
	}
	if (Buffer::SetDataWithResize<vk::DeviceAddress>(context, vertex_position_buffer, vertex_position, batch_size, create_info, allocation_info)) {
		vertex_position_buffer.SetName(context, "Vertex Position Buffer");
	}
	if (Buffer::SetDataWithResize<vk::DeviceAddress>(context, vertex_other_buffer, vertex_other, batch_size, create_info, allocation_info)) {
		vertex_other_buffer.SetName(context, "Vertex Other Buffer");
	}
}
