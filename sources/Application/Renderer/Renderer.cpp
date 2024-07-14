#include "Application/Renderer/Renderer.h"
#include "acceleration-structure.h"
#include "Application/Renderer/RenderContext.h"
#include <stdexcept>
#include "Application/Renderer/Renderer.h"
#include "Core/VulkanUtils.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Render.h"
#include "Application/Renderer/RenderContext.h"
#include "Application/Renderer/Renderer.h"
#include "file-system.h"
#include "Core/VulkanUtils.h"
#include "math-utils.h"
#include "acceleration-structure.h"
#include "Application/Renderer/RenderContext.h"

Renderer::Renderer(const DeviceContext& context, const RenderContext& render)
{
	swapchain = std::make_unique<Swapchain>(context);
	rt_image = std::make_unique<Image>(context, vk::ImageCreateInfo(
		{}, vk::ImageType::e2D, context.GetSurfaceFormat().format,
		vk::Extent3D(swapchain->GetExtent(), 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc));
	rt_image_view = context.GetDevice().createImageView(vk::ImageViewCreateInfo({}, *rt_image,
		vk::ImageViewType::e2D, context.GetSurfaceFormat().format,
		vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA),
		vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
	));

	UploadDescriptorSet(context, render);
	CreateRayTracingPipelineLayout(context);
	CreateRayTracingPipeline(context);
	shader_binding_table = std::make_unique<RayTracingShaderBindingTable>(context, ray_tracing_pipeline);
	command_pool = context.GetDevice().createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, context.GetGrpahicsQueueIndex()));
	CreateSyncObjects(context);
}

void Renderer::CmdInsertImageBarrier(const vk::CommandBuffer cmd, vk::ImageMemoryBarrier barrier) {
	barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
	barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
	cmd.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, {}, {}, {}, barrier);
}

void Renderer::UpdateDescriptorSet(const DeviceContext& context, const RenderContext& render, const vk::DescriptorSet& set) {
	vk::WriteDescriptorSetAccelerationStructureKHR write_tlas(render.tlas);
	vk::DescriptorBufferInfo write_instance_buffer(render.instances.GetBuffer(), 0, vk::WholeSize);
	vk::DescriptorBufferInfo write_constants_buffer(render.constants_buffer, 0, vk::WholeSize);
	std::vector<vk::WriteDescriptorSet> writes = {
		vk::WriteDescriptorSet(set, 0, 0, 1, vk::DescriptorType::eAccelerationStructureKHR, {}, {}, {}, &write_tlas),
		vk::WriteDescriptorSet(set, 5, 0, vk::DescriptorType::eStorageBuffer, {}, write_instance_buffer),
		vk::WriteDescriptorSet(set, 6, 0, vk::DescriptorType::eUniformBuffer, {}, write_constants_buffer),
	};
	context.GetDevice().updateDescriptorSets(writes, {});
}

vk::CommandBuffer Renderer::CreateFrameCommandBuffer(const DeviceContext& context, const vk::CommandPool pool, const uint32_t index) {
	vk::Device device = context.GetDevice();
	vk::CommandBufferAllocateInfo allocate_info(pool, vk::CommandBufferLevel::ePrimary, 1);
	vk::CommandBuffer cmd = device.allocateCommandBuffers(allocate_info)[0];

	vk::ImageSubresourceRange subresource(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
	vk::ImageCopy copy_region(
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
		vk::Offset3D(0, 0, 0),
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
		vk::Offset3D(0, 0, 0),
		vk::Extent3D(swapchain->GetExtent(), 1)
	);

	vk::Image target = swapchain->GetImage(index);

	cmd.begin(vk::CommandBufferBeginInfo());
	CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral,
		{}, {}, *rt_image, subresource));
	cmd.bindPipeline(vk::PipelineBindPoint::eRayTracingKHR, ray_tracing_pipeline);
	cmd.bindDescriptorSets(vk::PipelineBindPoint::eRayTracingKHR, pipeline_layout, 0, vk::ArrayProxy(descriptor_set), {});
	cmd.traceRaysKHR(
		shader_binding_table->rgen,
		shader_binding_table->miss,
		shader_binding_table->hit,
		shader_binding_table->call,
		swapchain->GetExtent().width,
		swapchain->GetExtent().height,
		1
	);
	CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
		{}, {}, target, subresource));
	CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eShaderWrite, vk::AccessFlagBits::eShaderRead,
		vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal,
		{}, {}, *rt_image, subresource));
	cmd.copyImage(*rt_image, vk::ImageLayout::eTransferSrcOptimal, target, vk::ImageLayout::eTransferDstOptimal, copy_region);
	CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
		vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderWrite,
		vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR,
		{}, {}, target, subresource));
	cmd.end();
	return cmd;
}

void Renderer::CreateSyncObjects(const DeviceContext& context) {
	vk::SemaphoreCreateInfo semaphore_create_info;
	vk::FenceCreateInfo fence_create_info(vk::FenceCreateFlagBits::eSignaled);
	image_available_semaphore = context.GetDevice().createSemaphore(semaphore_create_info);
	render_finished_semaphore = context.GetDevice().createSemaphore(semaphore_create_info);
	in_flight_fence = context.GetDevice().createFence(fence_create_info);
}

void Renderer::Draw(const DeviceContext& context, const RenderContext& render) {
	VK_CHECK(context.GetDevice().waitForFences({ in_flight_fence }, vk::True, UINT64_MAX));
	context.GetDevice().resetFences({ in_flight_fence });

	UpdateDescriptorSet(context, render, descriptor_set);
	uint32_t image_index = context.GetDevice().acquireNextImageKHR(*swapchain, UINT64_MAX, image_available_semaphore).value;
	vk::CommandBuffer cmd = CreateFrameCommandBuffer(context, command_pool, image_index);

	vk::PipelineStageFlags stage_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	vk::SubmitInfo submit_info(
		image_available_semaphore,
		stage_mask,
		cmd,
		render_finished_semaphore
	);
	context.GetGraphicsQueue().submit(submit_info, in_flight_fence);
	context.GetGraphicsQueue().waitIdle(); // 等待当前帧执行完毕， TODO: 优化之
	context.GetDevice().freeCommandBuffers(command_pool, cmd);
	std::vector<vk::SwapchainKHR> swapchains = { *swapchain };
	vk::PresentInfoKHR present_info(render_finished_semaphore, swapchains, image_index);
	VK_CHECK(context.GetPresentQueue().presentKHR(present_info));
}

vk::DescriptorPool Renderer::CreateDescriptorPool(const vk::Device device) {
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

vk::DescriptorSetLayout Renderer::CreateDescriptorSetLayout(const vk::Device device) {
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

void Renderer::UploadDescriptorSet(const DeviceContext& context, const RenderContext& render) {
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

void Renderer::CreateRayTracingPipelineLayout(const DeviceContext& context) {
	vk::PipelineLayoutCreateInfo create_info({}, descriptor_set_layout);
	pipeline_layout = context.GetDevice().createPipelineLayout(create_info);
}

void Renderer::CreateRayTracingPipeline(const DeviceContext& context) {
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

void Renderer::Destroy(const DeviceContext& context) {
	vk::Device device = context.GetDevice();
	device.waitIdle();
	device.destroySemaphore(image_available_semaphore);
	device.destroySemaphore(render_finished_semaphore);
	device.destroyFence(in_flight_fence);
	device.destroyCommandPool(command_pool);
	device.destroyImageView(rt_image_view);
	rt_image->Destroy(context);
	device.destroyDescriptorSetLayout(descriptor_set_layout);
	device.destroyDescriptorPool(descriptor_pool);
	shader_binding_table->Destroy(context);
	device.destroyPipeline(ray_tracing_pipeline);
	device.destroyPipelineLayout(pipeline_layout);
	swapchain->Destroy(context);
}
