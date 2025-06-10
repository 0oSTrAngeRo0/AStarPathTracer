import Core:DeviceContext;

#include "Core/Mesh.h"

Mesh::Mesh(
	const DeviceContext& context,
	const std::vector<glm::vec3>& positions, // vertex positions
	const std::vector<glm::vec3>& normals, // per vertex normal
	const std::vector<glm::vec4>& tangents, // per vertex tangent, glm::vec3 bitangent = cross(normal, tangent.xyz) * tangent.w
	const std::vector<glm::vec2>& uvs, // per vertex uv
	const std::vector<glm::uvec3>& indices // triangles
) {
	uint32_t count = positions.size();
	assert(count == normals.size() && count == tangents.size() && count == uvs.size());
	std::vector<VertexOther> vertex_other;
	for (uint32_t i = 0; i < count; i++) {
		vertex_other.emplace_back(VertexOther(normals[i], tangents[i], uvs[i]));
	}

	vk::BufferCreateInfo vertex_position_ci({}, {}, 
		vk::BufferUsageFlagBits::eVertexBuffer |
		vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
		vk::BufferUsageFlagBits::eShaderDeviceAddress);
	vertex_position_buffer = Buffer::CreateWithStaging<const VertexPosition>(context, vertex_position_ci, positions);

	vk::BufferCreateInfo vertex_otehr_ci({}, {},
		vk::BufferUsageFlagBits::eStorageBuffer |
		vk::BufferUsageFlagBits::eShaderDeviceAddress);
	vertex_other_buffer = Buffer::CreateWithStaging<VertexOther>(context, vertex_otehr_ci, vertex_other);

	vk::BufferCreateInfo index_ci({}, {},
		vk::BufferUsageFlagBits::eIndexBuffer |
		vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
		vk::BufferUsageFlagBits::eShaderDeviceAddress);
	index_buffer = Buffer::CreateWithStaging<const Index>(context, index_ci, indices);
}

void Mesh::Destroy(const DeviceContext& context) {
	context.GetDevice().destroyAccelerationStructureKHR(blas);
	as_buffer.Destroy(context);
	vertex_position_buffer.Destroy(context);
	vertex_other_buffer.Destroy(context);
	index_buffer.Destroy(context);
}

void Mesh::SetName(const DeviceContext& context, const std::string& name) {
	vertex_position_buffer.SetName(context, name + " Vertex Position Buffer");
	vertex_other_buffer.SetName(context, name + " Vertex Other Buffer");
	index_buffer.SetName(context, name + " Index Buffer");
}
