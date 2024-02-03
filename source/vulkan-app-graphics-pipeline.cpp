#include "vulkan-app.h"
#include "file-system.h"

VkShaderModule CreateShaderModule(const VkDevice &device, const std::vector<char> &code);

void CreatePipelineLayout(const VkDevice &device, VkPipelineLayout *pipeline_layout);

void CreateRenderPass(const VkFormat &format, const VkDevice &device, VkRenderPass *render_pass);

void VulkanApp::CreateGraphicsPipeline() {
    std::vector<char> vertex_shader_code = asset::LoadBinaryFile(R"(D:\C++\Projects\PathTracer\shaders\sample-vertex.spv)");
    std::vector<char> fragment_shader_code = asset::LoadBinaryFile(R"(D:\C++\Projects\PathTracer\shaders\sample-fragment.spv)");

    VkShaderModule vertex_shader = CreateShaderModule(device, vertex_shader_code);
    VkShaderModule fragment_shader = CreateShaderModule(device, fragment_shader_code);

    VkPipelineShaderStageCreateInfo vertex_create_info{};
    vertex_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_create_info.module = vertex_shader;
    vertex_create_info.pName = "main";
    vertex_create_info.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo fragment_create_info{};
    fragment_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_create_info.module = fragment_shader;
    fragment_create_info.pName = "main";
    fragment_create_info.pSpecializationInfo = nullptr;

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {
            vertex_create_info,
            fragment_create_info,
    };

    std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
    dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_create_info.dynamicStateCount = dynamic_states.size();
    dynamic_state_create_info.pDynamicStates = dynamic_states.data();

    VkPipelineVertexInputStateCreateInfo vertex_input_create_info{};
    vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_create_info.vertexBindingDescriptionCount = 0;
    vertex_input_create_info.pVertexBindingDescriptions = nullptr;
    vertex_input_create_info.vertexAttributeDescriptionCount = 0;
    vertex_input_create_info.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
    input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapchain_info.extent.width;
    viewport.height = (float) swapchain_info.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain_info.extent;

    VkPipelineViewportStateCreateInfo viewport_create_info{};
    viewport_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_create_info.viewportCount = 1;
    viewport_create_info.pViewports = &viewport;
    viewport_create_info.scissorCount = 1;
    viewport_create_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterization_create_info{};
    rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_create_info.depthClampEnable = VK_FALSE;
    rasterization_create_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_create_info.lineWidth = 1.0f;
    rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterization_create_info.depthBiasEnable = VK_FALSE;
    rasterization_create_info.depthBiasConstantFactor = 0.0f;
    rasterization_create_info.depthBiasClamp = 0.0f;
    rasterization_create_info.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisample_create_info{};
    multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_create_info.sampleShadingEnable = VK_FALSE;
    multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_create_info.minSampleShading = 1.0f;
    multisample_create_info.pSampleMask = nullptr;
    multisample_create_info.alphaToCoverageEnable = VK_FALSE;
    multisample_create_info.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blend_create_info{};
    color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_create_info.logicOpEnable = VK_FALSE;
    color_blend_create_info.logicOp = VK_LOGIC_OP_COPY; // Optional
    color_blend_create_info.attachmentCount = 1;
    color_blend_create_info.pAttachments = &color_blend_attachment;
    color_blend_create_info.blendConstants[0] = 0.0f; // Optional
    color_blend_create_info.blendConstants[1] = 0.0f; // Optional
    color_blend_create_info.blendConstants[2] = 0.0f; // Optional
    color_blend_create_info.blendConstants[3] = 0.0f; // Optional

    CreateRenderPass(swapchain_info.format, device, &render_pass);
    CreatePipelineLayout(device, &pipeline_layout);

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shader_stages.size();
    pipelineInfo.pStages = shader_stages.data();
    pipelineInfo.pVertexInputState = &vertex_input_create_info;
    pipelineInfo.pInputAssemblyState = &input_assembly_create_info;
    pipelineInfo.pViewportState = &viewport_create_info;
    pipelineInfo.pRasterizationState = &rasterization_create_info;
    pipelineInfo.pMultisampleState = &multisample_create_info;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &color_blend_create_info;
    pipelineInfo.pDynamicState = &dynamic_state_create_info;
    pipelineInfo.layout = pipeline_layout;
    pipelineInfo.renderPass = render_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphics_pipeline);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipelines!");
    }

    vkDestroyShaderModule(device, vertex_shader, nullptr);
    vkDestroyShaderModule(device, fragment_shader, nullptr);
}

VkShaderModule CreateShaderModule(const VkDevice &device, const std::vector<char> &code) {
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());
    VkShaderModule shader;
    VkResult result = vkCreateShaderModule(device, &create_info, nullptr, &shader);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module!");
    }
    return shader;
}

void CreatePipelineLayout(const VkDevice &device, VkPipelineLayout *pipeline_layout) {
    VkPipelineLayoutCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    create_info.setLayoutCount = 0; // Optional
    create_info.pSetLayouts = nullptr; // Optional
    create_info.pushConstantRangeCount = 0; // Optional
    create_info.pPushConstantRanges = nullptr; // Optional

    VkResult result = vkCreatePipelineLayout(device, &create_info, nullptr, pipeline_layout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }
}

void CreateRenderPass(const VkFormat &format, const VkDevice &device, VkRenderPass *render_pass) {
    VkAttachmentDescription color_attachment{};
    color_attachment.format = format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_create_info{};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &color_attachment;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;
    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies = &dependency;

    VkResult result = vkCreateRenderPass(device, &render_pass_create_info, nullptr, render_pass);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
}