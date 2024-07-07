#include "ray-tracing-shader-binding-table.h"
#include "Core/DeviceContext.h"
#include "Core/Buffer.h"
#include "math-utils.h"

RayTracingBindingTable::RayTracingBindingTable(const DeviceContext& context, vk::Pipeline pipeline) {
	auto& properties = context.GetGpuProperty<vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
	uint32_t rgen_count = 1;
	uint32_t miss_count = 1;
	uint32_t hit_count = 1;
	uint32_t handle_count = rgen_count + miss_count + hit_count;
	uint32_t handle_size = properties.shaderGroupHandleSize;
	uint32_t handle_size_aligned = math::align_up(handle_size, properties.shaderGroupHandleAlignment);
	size_t data_size = handle_size * handle_count;

	rgen.stride = math::align_up(handle_size_aligned, properties.shaderGroupBaseAlignment);
	rgen.size = rgen.stride;
	miss.stride = handle_size_aligned;
	miss.size = math::align_up(miss_count * handle_size_aligned, properties.shaderGroupBaseAlignment);
	hit.stride = handle_size_aligned;
	hit.size = math::align_up(hit_count * handle_size_aligned, properties.shaderGroupBaseAlignment);

	size_t sbt_size = rgen.size + miss.size + hit.size + call.size;
	std::vector<uint8_t> target(sbt_size);
	{
		std::vector<uint8_t> handles = context.GetDevice().getRayTracingShaderGroupHandlesKHR<uint8_t>(pipeline, 0, handle_count, data_size);
		auto getHandle = [&](int i) { return handles.data() + i * handle_size; };
		uint8_t* target_raw = target.data();
		uint8_t* data = nullptr;
		uint32_t handle_index = 0;
		data = target_raw;
		memcpy(data, getHandle(handle_index++), handle_size);
		data = target_raw + rgen.size;
		for (uint32_t i = 0; i < miss_count; i++)
		{
			memcpy(data, getHandle(handle_index++), handle_size);
			data += miss.stride;
		}
		data = target_raw + rgen.size + miss.size;
		for (uint32_t i = 0; i < hit_count; i++)
		{
			memcpy(data, getHandle(handle_index++), handle_size);
			data += hit.stride;
		}
	}

	vk::BufferCreateInfo buffer_ci({}, {},
		vk::BufferUsageFlagBits::eTransferSrc |
		vk::BufferUsageFlagBits::eShaderDeviceAddress |
		vk::BufferUsageFlagBits::eShaderBindingTableKHR);
	buffer = Buffer::CreateWithData<uint8_t>(context, buffer_ci, target);
	buffer.SetName(context, "Shader Binding Table");

	vk::DeviceAddress sbt_address = buffer.GetDeviceAddress();
	rgen.deviceAddress = sbt_address;
	miss.deviceAddress = sbt_address + rgen.size;
	hit.deviceAddress = sbt_address + rgen.size + miss.size;
}

void RayTracingBindingTable::Destroy(const DeviceContext& context)
{
	buffer.Destroy(context);
}
