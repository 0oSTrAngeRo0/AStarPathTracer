import Core:DeviceContext;
import Core:Buffer;
#include "Core/RayTracingShaders.h"
#include <fstream>

std::vector<std::byte> RayTracingShaders::LoadBinaryFile(const std::string filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to load file!");
	}
	size_t file_size = file.tellg();
	std::vector<char> buffer(file_size);
	file.seekg(0);
	file.read(buffer.data(), file_size);
	file.close();
	return reinterpret_cast<std::vector<std::byte>&>(buffer);
}

RayTracingShaders::BindingTable::BindingTable(
	const DeviceContext& context, 
	const vk::Pipeline pipeline, 
	const ShaderCount& counts
) {
	auto& properties = context.GetGpuProperty<vk::PhysicalDeviceRayTracingPipelinePropertiesKHR>();
	uint32_t rgen_count = counts.GetRayGen();
	uint32_t miss_count = counts.GetMiss();
	uint32_t hit_count = counts.GetClosestHit();
	uint32_t call_count = counts.GetCallable();
	uint32_t handle_count = rgen_count + miss_count + hit_count + call_count;
	uint32_t handle_size = properties.shaderGroupHandleSize;
	uint32_t handle_size_aligned = AlignUp(handle_size, properties.shaderGroupHandleAlignment);
	size_t data_size = handle_size * handle_count;

	rgen.stride = AlignUp(handle_size_aligned, properties.shaderGroupBaseAlignment);
	rgen.size = rgen.stride;
	miss.stride = handle_size_aligned;
	miss.size = AlignUp(miss_count * handle_size_aligned, properties.shaderGroupBaseAlignment);
	hit.stride = handle_size_aligned;
	hit.size = AlignUp(hit_count * handle_size_aligned, properties.shaderGroupBaseAlignment);
	call.stride = handle_size_aligned;
	call.size = AlignUp(call_count * handle_size_aligned, properties.shaderGroupBaseAlignment);

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
		for (uint32_t i = 0; i < miss_count; i++) {
			memcpy(data, getHandle(handle_index++), handle_size);
			data += miss.stride;
		}
		data = target_raw + rgen.size + miss.size;
		for (uint32_t i = 0; i < hit_count; i++) {
			memcpy(data, getHandle(handle_index++), handle_size);
			data += hit.stride;
		}

		data = target_raw + rgen.size + miss.size + hit.size;
		for (uint32_t i = 0; i < call_count; i++) {
			memcpy(data, getHandle(handle_index++), handle_size);
			data += call.stride;
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
	call.deviceAddress = sbt_address + rgen.size + miss.size + hit.size;
}

RayTracingShaders::PipelineData::PipelineData(
	const DeviceContext& context, 
	const std::vector<ShaderData>& shaders
) {
	assert(std::is_sorted(shaders.begin(), shaders.end(), StageComparer));

	for (size_t i = 0, end = shaders.size(); i < end; i++) {
		const auto& shader = shaders[i];

		IncreaseCount(shader.stage);

		// emplace module
		std::vector<std::byte> binary_code = LoadBinaryFile(shader.spv_path.c_str());
		std::vector<uint32_t> compiled_code = reinterpret_cast<std::vector<uint32_t>&>(binary_code);
		vk::ShaderModule module = context.GetDevice().createShaderModule(vk::ShaderModuleCreateInfo({}, compiled_code));
		modules.emplace_back(module);

		// emplace stage
		stages.emplace_back(vk::PipelineShaderStageCreateInfo({}, shader.stage, module, shader.entry_function.c_str()));

		// emplace group
		if (shader.stage == vk::ShaderStageFlagBits::eClosestHitKHR ||
			shader.stage == vk::ShaderStageFlagBits::eAnyHitKHR ||
			shader.stage == vk::ShaderStageFlagBits::eIntersectionKHR) {
			groups.emplace_back(vk::RayTracingShaderGroupCreateInfoKHR(vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup, vk::ShaderUnusedKHR, i));
		}
		else {
			groups.emplace_back(vk::RayTracingShaderGroupCreateInfoKHR(vk::RayTracingShaderGroupTypeKHR::eGeneral, i));
		}
	}
}

void RayTracingShaders::PipelineData::Destroy(const DeviceContext& context) {
	for (auto& module : modules) {
		context.GetDevice().destroyShaderModule(module);
	}
}


void RayTracingShaders::PipelineData::IncreaseCount(const vk::ShaderStageFlagBits stage) {
	if (stage == vk::ShaderStageFlagBits::eClosestHitKHR) {
		count.hit++;
	}
	else if (stage == vk::ShaderStageFlagBits::eRaygenKHR) {
		count.rgen++;
	}
	else if (stage == vk::ShaderStageFlagBits::eCallableKHR) {
		count.call++;
	}
	else if (stage == vk::ShaderStageFlagBits::eMissKHR) {
		count.miss++;
	}
}
