#include "acceleration-structure.h"
#include <stdexcept>
#include "model-loader.h"

void AccelerationStructure::Create(VkDevice const& device,
	const VkPhysicalDevice& physical_device,
	const VkCommandBuffer& command_buffer,
	const std::vector<GpuMesh>& meshes) {

	const uint32_t mesh_count = meshes.size();

	blases.resize(mesh_count);
	for (int i = 0; i < mesh_count; ++i) {
		blases[i].Create(meshes[i], command_buffer, device, physical_device);
	}

	std::vector<VkAccelerationStructureInstanceKHR> instances(mesh_count);
	for (int i = 0; i < (int)instances.size(); i++) {
		glm::mat3x4 matrix;
		memcpy(&instances[i].transform.matrix[0][0], &matrix[0][0], 12 * sizeof(float));
		instances[i].instanceCustomIndex = i;
		instances[i].mask = 0xff;
		instances[i].instanceShaderBindingTableRecordOffset = 0;
		instances[i].flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		instances[i].accelerationStructureReference = blases[i].device_address;
	}

	const uint32_t buffer_type = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
	VkDeviceSize instance_buffer_size = instances.size() * sizeof(VkAccelerationStructureInstanceKHR);
	instance_buffer.Create(physical_device, device, instance_buffer_size, buffer_type);

	tlas.Create(command_buffer, device, physical_device, instance_buffer.device_address, mesh_count);

}

void AccelerationStructure::Destroy(VkDevice const& device) {
	tlas.Destroy(device);
	instance_buffer.Destroy(device);
	for (BlasInfo& blas : blases) {
		blas.Destroy(device);
	}
}

void CreateAccelerationStructure(const VkDevice& device,
	const VkDeviceSize& size,
	const VkBuffer& buffer,
	VkAccelerationStructureTypeKHR type,
	VkAccelerationStructureKHR* acceleration_structure) {
	VkAccelerationStructureCreateInfoKHR create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	create_info.buffer = buffer;
	create_info.offset = 0;
	create_info.size = size;
	create_info.type = type;
	VkResult result = vkCreateAccelerationStructureKHR(device, &create_info, nullptr, acceleration_structure);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create acceleration structure!");
	}
}

VkAccelerationStructureGeometryKHR GetBlasGeometry(const GpuMesh mesh) {
	VkAccelerationStructureGeometryTrianglesDataKHR triangles_data;
	triangles_data.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	triangles_data.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
	triangles_data.vertexData.deviceAddress = mesh.vertex_buffer.device_address;
	triangles_data.vertexStride = sizeof(asset::Vertex);
	triangles_data.maxVertex = mesh.vertex_count - 1;
	triangles_data.indexType = VK_INDEX_TYPE_UINT32;
	triangles_data.indexData.deviceAddress = mesh.index_buffer.device_address;

	VkAccelerationStructureGeometryDataKHR geometry_data;
	geometry_data.triangles = triangles_data;

	VkAccelerationStructureGeometryKHR geometry;
	geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	geometry.geometry = geometry_data;

	return geometry;
}

VkAccelerationStructureGeometryKHR GetTlasGeometry(const VkDeviceAddress& device_address) {
	VkDeviceOrHostAddressConstKHR data;
	data.deviceAddress = device_address;

	VkAccelerationStructureGeometryInstancesDataKHR instance;
	instance.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	instance.arrayOfPointers = VK_FALSE;
	instance.data = data;

	VkAccelerationStructureGeometryDataKHR geometry_data;
	geometry_data.instances = instance;

	VkAccelerationStructureGeometryKHR geometry;
	geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	geometry.geometry = geometry_data;

	return geometry;
}

void BlasInfo::Create(const GpuMesh& mesh,
	const VkCommandBuffer& command_buffer,
	const VkDevice& device,
	const VkPhysicalDevice& physical_device) {
	VkAccelerationStructureGeometryKHR geometry = GetBlasGeometry(mesh);

	VkAccelerationStructureBuildGeometryInfoKHR geometry_info{};
	geometry_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	geometry_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	geometry_info.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	geometry_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	geometry_info.pGeometries = &geometry;
	geometry_info.geometryCount = 1;
	geometry_info.pNext = nullptr;

	VkAccelerationStructureBuildSizesInfoKHR build_sizes{};
	build_sizes.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	build_sizes.pNext = nullptr;
	uint32_t triangle_count = mesh.index_count / 3;
	VkAccelerationStructureBuildTypeKHR build_type = VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR;
	vkGetAccelerationStructureBuildSizesKHR(device, build_type, &geometry_info, &triangle_count, &build_sizes);

	VkBufferUsageFlagBits buffer_usage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
	buffer.Create(physical_device, device, build_sizes.accelerationStructureSize, buffer_usage);
	CreateAccelerationStructure(device,
		build_sizes.accelerationStructureSize,
		buffer.handle,
		VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR,
		&acceleration_structure);

	Buffer scratch_buffer;
	buffer.Create(physical_device, device, build_sizes.buildScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

	geometry_info.dstAccelerationStructure = acceleration_structure;
	geometry_info.scratchData.deviceAddress = scratch_buffer.device_address;

	VkAccelerationStructureBuildRangeInfoKHR build_range_info{};
	build_range_info.primitiveCount = mesh.index_count / 3;
	std::vector<VkAccelerationStructureBuildRangeInfoKHR*> build_range_infos = { &build_range_info, };

	uint32_t info_count = build_range_infos.size();
	VkAccelerationStructureBuildRangeInfoKHR** range_infos = build_range_infos.data();
	vkCmdBuildAccelerationStructuresKHR(command_buffer, info_count, &geometry_info, range_infos);

	VkAccelerationStructureDeviceAddressInfoKHR device_address_info{};
	device_address_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	device_address_info.accelerationStructure = acceleration_structure;
	device_address = vkGetAccelerationStructureDeviceAddressKHR(device, &device_address_info);

	scratch_buffer.Destroy(device);
}

void BlasInfo::Destroy(VkDevice const& device) const {
	vkDestroyAccelerationStructureKHR(device, acceleration_structure, nullptr);
	buffer.Destroy(device);
}

void TlasInfo::Create(const VkCommandBuffer& command_buffer,
	const VkDevice& device,
	const VkPhysicalDevice& physical_device,
	const VkDeviceAddress& device_address,
	const uint32_t instance_count) {

	VkAccelerationStructureGeometryKHR geometry = GetTlasGeometry(device_address);

	VkAccelerationStructureBuildGeometryInfoKHR geometry_info{};
	geometry_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	geometry_info.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	geometry_info.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	geometry_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	geometry_info.pGeometries = &geometry;
	geometry_info.geometryCount = 1;
	geometry_info.pNext = nullptr;

	VkAccelerationStructureBuildSizesInfoKHR build_sizes{};
	build_sizes.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	build_sizes.pNext = nullptr;
	VkAccelerationStructureBuildTypeKHR build_type = VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR;
	vkGetAccelerationStructureBuildSizesKHR(device, build_type, &geometry_info, &instance_count, &build_sizes);

	VkBufferUsageFlagBits buffer_usage = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
	buffer.Create(physical_device, device, build_sizes.accelerationStructureSize, buffer_usage);
	CreateAccelerationStructure(device,
		build_sizes.accelerationStructureSize,
		buffer.handle,
		VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR,
		&acceleration_structure);

	scratch_buffer.Create(physical_device, device, build_sizes.buildScratchSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

	geometry_info.dstAccelerationStructure = acceleration_structure;
	geometry_info.scratchData.deviceAddress = scratch_buffer.device_address;

	VkAccelerationStructureBuildRangeInfoKHR build_range_info{};
	build_range_info.primitiveCount = instance_count;
	std::vector<VkAccelerationStructureBuildRangeInfoKHR*> build_range_infos = { &build_range_info, };

	uint32_t info_count = build_range_infos.size();
	VkAccelerationStructureBuildRangeInfoKHR** range_infos = build_range_infos.data();
	vkCmdBuildAccelerationStructuresKHR(command_buffer, info_count, &geometry_info, range_infos);
}

void GpuMesh::Destroy(VkDevice const& device) {
	vertex_buffer.Destroy(device);
	index_buffer.Destroy(device);
}
