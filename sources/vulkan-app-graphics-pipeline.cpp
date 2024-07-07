#include "vulkan-app.h"
#include "file-system.h"
#include "vulkan-utils.h"
#include "math-utils.h"
#include "acceleration-structure.h"
#include "Core/RenderContext.h"

vk::DescriptorPool CreateDescriptorPool(const vk::Device device) {
	std::vector<vk::DescriptorPoolSize> pool_sizes = {
		vk::DescriptorPoolSize(vk::DescriptorType::eAccelerationStructureKHR, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1),
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1)
	};
	return device.createDescriptorPool(vk::DescriptorPoolCreateInfo({}, 1, pool_sizes));
}

vk::DescriptorSetLayout CreateDescriptorSetLayout(const vk::Device device) {
	std::vector<vk::DescriptorSetLayoutBinding> bindings = {
		vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eAccelerationStructureKHR, 1, vk::ShaderStageFlagBits::eRaygenKHR), // tlas
		vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eRaygenKHR), // output image
		vk::DescriptorSetLayoutBinding(2, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // vertex buffer
		vk::DescriptorSetLayoutBinding(3, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // index buffer
		vk::DescriptorSetLayoutBinding(4, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // material buffer
		vk::DescriptorSetLayoutBinding(5, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eClosestHitKHR), // instance buffer
		vk::DescriptorSetLayoutBinding(6, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eRaygenKHR) // constants buffer
	};
	return device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, bindings));
}

void VulkanApp::UploadDescriptorSet(const DeviceContext& context, const RenderContext& render) {
	descriptor_pool = CreateDescriptorPool(context.GetDevice());
	descriptor_set_layout = CreateDescriptorSetLayout(context.GetDevice());
	std::vector<vk::DescriptorSet> sets = context.GetDevice().allocateDescriptorSets(vk::DescriptorSetAllocateInfo(descriptor_pool, descriptor_set_layout));
	descriptor_set = sets[0];

	vk::WriteDescriptorSetAccelerationStructureKHR write_tlas(render.tlas);
	vk::DescriptorImageInfo write_rt_image({}, rt_image_view, vk::ImageLayout::eGeneral);
	vk::DescriptorBufferInfo write_vertex_buffer(render.vertex_buffer, 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_index_buffer(render.index_buffer, 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_material_buffer(render.material_buffer, 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_instance_buffer(render.instances.GetBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_constants_buffer(render.constants_buffer, 0, vk::WholeSize);
	std::vector<vk::WriteDescriptorSet> writes = {
		vk::WriteDescriptorSet(descriptor_set, 0, 0, 1, vk::DescriptorType::eAccelerationStructureKHR, {}, {}, {}, &write_tlas),
		vk::WriteDescriptorSet(descriptor_set, 1, 0, vk::DescriptorType::eStorageImage, write_rt_image),
		vk::WriteDescriptorSet(descriptor_set, 2, 0, vk::DescriptorType::eStorageBuffer, {}, write_vertex_buffer),
		vk::WriteDescriptorSet(descriptor_set, 3, 0, vk::DescriptorType::eStorageBuffer, {}, write_index_buffer),
		vk::WriteDescriptorSet(descriptor_set, 4, 0, vk::DescriptorType::eStorageBuffer, {}, write_material_buffer),
		vk::WriteDescriptorSet(descriptor_set, 5, 0, vk::DescriptorType::eStorageBuffer, {}, write_instance_buffer),
		vk::WriteDescriptorSet(descriptor_set, 6, 0, vk::DescriptorType::eUniformBuffer, {}, write_constants_buffer),
	};
	context.GetDevice().updateDescriptorSets(writes, {});
}

void VulkanApp::CreateRayTracingPipelineLayout(const DeviceContext& context) {
	vk::PipelineLayoutCreateInfo create_info({}, descriptor_set_layout);
	pipeline_layout = context.GetDevice().createPipelineLayout(create_info);
}

void VulkanApp::CreateRayTracingPipeline(const DeviceContext& context) {
	std::vector<uint32_t> ray_gen_shader_code = asset::LoadBinaryFile<uint32_t>(R"(D:\C++\Projects\PathTracer\shaders\sample.rgen.spv)");
	std::vector<uint32_t> miss_shader_code = asset::LoadBinaryFile<uint32_t>(R"(D:\C++\Projects\PathTracer\shaders\sample.rmiss.spv)");
	std::vector<uint32_t> closest_hit_shader_code = asset::LoadBinaryFile<uint32_t>(R"(D:\C++\Projects\PathTracer\shaders\sample.rchit.spv)");

	vk::ShaderModule ray_gen_shader = context.GetDevice().createShaderModule(vk::ShaderModuleCreateInfo({}, ray_gen_shader_code));
	vk::ShaderModule miss_shader = context.GetDevice().createShaderModule(vk::ShaderModuleCreateInfo({}, miss_shader_code));
	vk::ShaderModule closest_hit_shader = context.GetDevice().createShaderModule(vk::ShaderModuleCreateInfo({}, closest_hit_shader_code));


	std::vector<vk::PipelineShaderStageCreateInfo> stages = {
		vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eRaygenKHR, ray_gen_shader, "main"),
		vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eMissKHR, miss_shader, "main"),
		vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eClosestHitKHR, closest_hit_shader, "main"),
	};
	std::vector<vk::RayTracingShaderGroupCreateInfoKHR> groups = {
		vk::RayTracingShaderGroupCreateInfoKHR(vk::RayTracingShaderGroupTypeKHR::eGeneral, 0),
		vk::RayTracingShaderGroupCreateInfoKHR(vk::RayTracingShaderGroupTypeKHR::eGeneral, 1),
		vk::RayTracingShaderGroupCreateInfoKHR(vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup, vk::ShaderUnusedKHR, 2),
	};
	vk::RayTracingPipelineCreateInfoKHR create_info({}, stages, groups, 1, {}, {}, {}, pipeline_layout);
	ray_tracing_pipeline = context.GetDevice().createRayTracingPipelineKHR({}, {}, create_info).value;

	context.GetDevice().destroyShaderModule(ray_gen_shader);
	context.GetDevice().destroyShaderModule(miss_shader);
	context.GetDevice().destroyShaderModule(closest_hit_shader);
}
