#pragma

import vulkan_hpp;

class CommandUtilities {
public:
	static void CmdInsertImageBarrier(const vk::CommandBuffer cmd, vk::ImageMemoryBarrier barrier) {
		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		cmd.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, {}, {}, {}, barrier);
	}
};