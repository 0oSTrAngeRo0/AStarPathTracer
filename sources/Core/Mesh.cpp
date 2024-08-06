#include "Core/Mesh.h"
#include "Core/DeviceContext.h"

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
	std::vector<Vertex> vertices;
	for (uint32_t i = 0; i < count; i++) {
		vertices.emplace_back(Vertex(positions[i], normals[i], tangents[i], uvs[i]));
	}

	vk::BufferCreateInfo vertex_ci({}, {},
		vk::BufferUsageFlagBits::eVertexBuffer |
		vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
		vk::BufferUsageFlagBits::eShaderDeviceAddress);
	vertex_buffer = Buffer::CreateWithStaging<Vertex>(context, vertex_ci, vertices);

	vk::BufferCreateInfo index_ci({}, {},
		vk::BufferUsageFlagBits::eIndexBuffer |
		vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
		vk::BufferUsageFlagBits::eShaderDeviceAddress);
	index_buffer = Buffer::CreateWithStaging<const glm::uvec3>(context, index_ci, indices);
}

void Mesh::Destroy(const DeviceContext& context) {
	context.GetDevice().destroyAccelerationStructureKHR(blas);
	as_buffer.Destroy(context);
	vertex_buffer.Destroy(context);
	index_buffer.Destroy(context);
}

void Mesh::SetName(const DeviceContext& context, const std::string& name) {
	vertex_buffer.SetName(context, name + " Vertex Buffer");
	index_buffer.SetName(context, name + " Index Buffer");
}
