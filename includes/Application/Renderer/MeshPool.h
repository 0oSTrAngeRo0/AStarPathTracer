#pragma once

#include <unordered_map>
#include <vector>

#include "Engine/Guid.h"
#include "Core/Mesh.h"
#include "Core/Buffer.h"

class DeviceContext;

class MeshPool {
public:
	const Mesh& GetMesh(const Uuid& device_id);
	void EnsureMeshes(const DeviceContext& context, const std::vector<std::tuple<Uuid, Uuid>>& used_meshes);
	void ReleaseUnusedMeshes(const DeviceContext& context, std::vector<Uuid> used);
	void Destroy(const DeviceContext& context);
	inline const Buffer& GetVertexPositionBuffer() const { return vertex_position_buffer; }
	inline const Buffer& GetVertexOtherBuffer() const { return vertex_other_buffer; }
	inline const Buffer& GetIndexBuffer() const { return index_buffer; }
private:
	std::unordered_map<Uuid, Mesh> meshes;
	Buffer vertex_position_buffer;
	Buffer vertex_other_buffer;
	Buffer index_buffer;
	void UpdateBuffers(const DeviceContext& context);
};