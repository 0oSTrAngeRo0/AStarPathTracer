#include "vulkan-app.h"
#include "file-system.h"
#include "vulkan-utils.h"

vk::RenderPass CreateRenderPass(const vk::Format& format, const vk::Device& device) {
	std::vector<vk::AttachmentDescription> color_attachments = {
		vk::AttachmentDescription(
			{},
			format,
			vk::SampleCountFlagBits::e1,
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			vk::AttachmentLoadOp::eDontCare,
			vk::AttachmentStoreOp::eDontCare,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::ePresentSrcKHR
		)
	};

	std::vector<vk::AttachmentReference> color_attachment_references = {
		vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal)
	};
	std::vector<vk::SubpassDescription> subpasses = {
		vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics, {}, color_attachment_references)
	};

	std::vector<vk::SubpassDependency> dependencies = {
		vk::SubpassDependency(
			vk::SubpassExternal,
			{},
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			{},
			vk::AccessFlagBits::eColorAttachmentWrite,
			{}
		),
	};

	vk::RenderPassCreateInfo render_pass_create_info({}, color_attachments, subpasses, dependencies);
	return device.createRenderPass(render_pass_create_info);
}

void VulkanApp::CreateGraphicsPipeline() {
	std::vector<uint32_t> vertex_shader_code = asset::LoadBinaryFile<uint32_t>(R"(D:\C++\Projects\PathTracer\shaders\sample-vertex.spv)");
	std::vector<uint32_t> fragment_shader_code = asset::LoadBinaryFile<uint32_t>(R"(D:\C++\Projects\PathTracer\shaders\sample-fragment.spv)");

	vk::ShaderModule vertex_shader = device.createShaderModule(vk::ShaderModuleCreateInfo({}, vertex_shader_code));
	vk::ShaderModule fragment_shader = device.createShaderModule(vk::ShaderModuleCreateInfo({}, fragment_shader_code));

	vk::PipelineShaderStageCreateInfo vertex_create_info({}, vk::ShaderStageFlagBits::eVertex, vertex_shader, "main");
	vk::PipelineShaderStageCreateInfo fragment_create_info({}, vk::ShaderStageFlagBits::eFragment, fragment_shader, "main");

	std::vector<vk::PipelineShaderStageCreateInfo> shader_stages = {
		vertex_create_info,
		fragment_create_info,
	};

	std::vector<vk::DynamicState> dynamic_states = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};
	vk::PipelineDynamicStateCreateInfo dynamic_state_create_info({}, dynamic_states);
	vk::PipelineVertexInputStateCreateInfo vertex_input_create_info;
	vk::PipelineInputAssemblyStateCreateInfo input_assembly_create_info({}, vk::PrimitiveTopology::eTriangleList, vk::False);

	std::vector<vk::Viewport> viewports = {
		vk::Viewport(0.0f, 0.0f, (float)swapchain_info.extent.width, (float)swapchain_info.extent.height, 0.0f, 1.0f)
	};
	std::vector<vk::Rect2D> scissors = {
		vk::Rect2D(vk::Offset2D(0, 0), swapchain_info.extent)
	};
	vk::PipelineViewportStateCreateInfo viewport_create_info({}, viewports, scissors);

	vk::PipelineRasterizationStateCreateInfo rasterization_create_info(
		{}, vk::False, vk::False,
		vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eBack,
		vk::FrontFace::eClockwise,
		vk::False, 0, 0, 0, 1);

	vk::PipelineMultisampleStateCreateInfo multisample_create_info({}, vk::SampleCountFlagBits::e1, vk::False, 1, {}, vk::False, vk::False);

	std::vector<vk::PipelineColorBlendAttachmentState> color_blend_attachments = {
		vk::PipelineColorBlendAttachmentState(vk::False,
			vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
			vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
		) 
	};
	vk::PipelineColorBlendStateCreateInfo color_blend_create_info({}, vk::False, vk::LogicOp::eCopy, color_blend_attachments, {});

	render_pass = CreateRenderPass(swapchain_info.format, device);
	pipeline_layout = device.createPipelineLayout(vk::PipelineLayoutCreateInfo());

	vk::GraphicsPipelineCreateInfo pipeline_create_info(
		{},
		shader_stages, 
		&vertex_input_create_info, 
		&input_assembly_create_info, 
		{},
		&viewport_create_info, 
		&rasterization_create_info,
		& multisample_create_info,
		{},
		& color_blend_create_info,
		& dynamic_state_create_info,
		pipeline_layout,
		render_pass,
		0,
		{},
		-1
	);

	graphics_pipeline = device.createGraphicsPipeline({}, pipeline_create_info).value;

	device.destroyShaderModule(vertex_shader);
	device.destroyShaderModule(fragment_shader);
}
