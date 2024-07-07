#include "Core/Mesh.h"
#include "Core/DeviceContext.h"

Mesh::Mesh(const DeviceContext& context, const vk::ArrayProxyNoTemporaries<Vertex>& vertices, const vk::ArrayProxyNoTemporaries<uint32_t>& indices) {
	vk::BufferCreateInfo vertex_ci({}, {},
		vk::BufferUsageFlagBits::eVertexBuffer |
		vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
		vk::BufferUsageFlagBits::eShaderDeviceAddress);
	vertex_buffer = Buffer::CreateWithStaging<Vertex>(context, vertex_ci, vertices);

	vk::BufferCreateInfo index_ci({}, {},
		vk::BufferUsageFlagBits::eIndexBuffer |
		vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR |
		vk::BufferUsageFlagBits::eShaderDeviceAddress);
	index_buffer = Buffer::CreateWithStaging<uint32_t>(context, index_ci, indices);
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
