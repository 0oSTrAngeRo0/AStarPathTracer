module Core;

TemporaryCommandBufferPool::TemporaryCommandBufferPool(const vk::Device device, const uint32_t queue_family_index) {
	vk::CommandPoolCreateInfo create_info(vk::CommandPoolCreateFlagBits::eTransient, queue_family_index);
	pool = device.createCommandPool(create_info);
}


const vk::CommandBuffer TemporaryCommandBufferPool::Get(const vk::Device device) const {
	vk::CommandBufferAllocateInfo allocate_info(pool, vk::CommandBufferLevel::ePrimary, 1);
	vk::CommandBuffer buffer = device.allocateCommandBuffers(allocate_info)[0];
	buffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
	return buffer;
}

void TemporaryCommandBufferPool::Release(const vk::CommandBuffer buffer, const vk::Device device, const vk::Queue queue) const {
	buffer.end();
	std::vector<vk::CommandBuffer> command_buffers = {
		buffer,
	};
	std::vector<vk::SubmitInfo> submit_infos = {
		vk::SubmitInfo({}, {}, command_buffers)
	};
	queue.submit(submit_infos);
	queue.waitIdle();
	device.freeCommandBuffers(pool, command_buffers);
}

void TemporaryCommandBufferPool::Destroy(const vk::Device device) {
	device.destroyCommandPool(pool);
}
