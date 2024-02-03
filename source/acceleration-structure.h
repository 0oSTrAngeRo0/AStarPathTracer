#ifndef PATHTRACER_SOURCE_ACCELERATION_STRUCTURE_H_
#define PATHTRACER_SOURCE_ACCELERATION_STRUCTURE_H_

#include "volk.h"
#include "buffer.h"
#include <vector>

struct GpuMesh {
	Buffer vertex_buffer;
	Buffer index_buffer;
	uint32_t vertex_count;
	uint32_t index_count;

	void Destroy(const VkDevice& device);
};

struct BlasInfo {
	VkAccelerationStructureKHR acceleration_structure;
	Buffer buffer;
	VkDeviceAddress device_address;

	void Create(const GpuMesh& mesh,
		const VkCommandBuffer& command_buffer,
		const VkDevice& device,
		const VkPhysicalDevice& physical_device);
	void Destroy(const VkDevice& device) const;
};

struct TlasInfo {
	VkAccelerationStructureKHR acceleration_structure;
	Buffer buffer;
	Buffer scratch_buffer;

	void Create(const VkCommandBuffer& command_buffer,
		const VkDevice& device,
		const VkPhysicalDevice& physical_device,
		const VkDeviceAddress& device_address,
		uint32_t instance_count);
	void Destroy(const VkDevice& device) {
		vkDestroyAccelerationStructureKHR(device, acceleration_structure, nullptr);
		buffer.Destroy(device);
		scratch_buffer.Destroy(device);
	}
};

struct AccelerationStructure {
	std::vector<BlasInfo> blases;
	TlasInfo tlas;
	Buffer instance_buffer;
	void Create(VkDevice const& device,
		const VkPhysicalDevice& physical_device,
		const VkCommandBuffer& command_buffer,
		const std::vector<GpuMesh>& meshes);
	void Destroy(const VkDevice& device);
};

#endif //PATHTRACER_SOURCE_ACCELERATION_STRUCTURE_H_
