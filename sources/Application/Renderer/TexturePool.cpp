#include "Core/DeviceContext.h"
#include "Application/Renderer/TexturePool.h"
#include "Application/Renderer/CommandUtilities.h"
#include "Engine/Resources/TextureResourceData.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Core/Buffer.h"

uint32_t TexturePool::GetOrAddHandle(const Uuid& id) {
	if (!id.isValid()) return InvalidHandle;
	auto it = texture_map.find(id);
	if (it != texture_map.end()) return it->second.index;

	size_t index = texture_array.size();

	TextureData data;
	data.is_loaded = false;
	data.index = index;
	texture_map.emplace(std::make_pair(id, data));

	texture_array.emplace_back();

	is_dirty = true;

	std::printf("Texture Asset Registerd: index:[%u], id:[%s]\n", index, id.str().c_str());
	return index;
}

void TexturePool::Arrange(std::vector<Uuid> used_textures) {}

void TexturePool::Ensure(const DeviceContext& context) {
	if (!is_dirty) return;
	vk::CommandBuffer cmd = context.GetTempCmd();
	std::vector<Buffer> staging_buffers;
	for (auto& pair : texture_map) {
		const auto& uuid = pair.first;
		auto& texture = texture_map[uuid];
		if (texture.is_loaded) continue;
		const auto& resource = ResourcesManager::GetInstance().GetResource(uuid);
		auto texture_data = LoadTextureResourceRegistry::Get(resource.GetResourceTypeId()).value()(resource);
		vk::BufferCreateInfo staging_buffer_ci({}, {}, vk::BufferUsageFlagBits::eTransferSrc);
		Buffer staging_buffer = Buffer::CreateWithData<std::byte>(context, staging_buffer_ci, texture_data.data);
		staging_buffers.emplace_back(staging_buffer);

		Image image(context, vk::ImageCreateInfo(
			{},
			vk::ImageType::e2D,
			vk::Format::eR8G8B8A8Srgb,
			vk::Extent3D(texture_data.width, texture_data.width, 1),
			1,
			1,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
		));

		CommandUtilities::CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
			vk::AccessFlagBits::eNone, vk::AccessFlagBits::eNone,
			vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
			{}, {}, image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
		));
		cmd.copyBufferToImage(
			staging_buffer,
			image,
			vk::ImageLayout::eTransferDstOptimal,
			vk::BufferImageCopy(
				0,
				0,
				0,
				vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
				vk::Offset3D(0, 0, 0),
				vk::Extent3D(texture_data.width, texture_data.height, 1)
			)
		);
		CommandUtilities::CmdInsertImageBarrier(cmd, vk::ImageMemoryBarrier(
			vk::AccessFlagBits::eNone, vk::AccessFlagBits::eShaderRead,
			vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
			{}, {}, image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
		));

		texture.is_loaded = true;
		texture.image = image;

		vk::DescriptorImageInfo& descriptor = texture_array[texture.index];
		descriptor.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		descriptor.imageView = context.GetDevice().createImageView(vk::ImageViewCreateInfo(
			{}, image, vk::ImageViewType::e2D, vk::Format::eR8G8B8A8Srgb,
			vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA),
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
		));
		descriptor.sampler = context.GetDevice().createSampler(vk::SamplerCreateInfo(
			{}, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eNearest,
			vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge,
			{}, vk::False, {}, vk::False, vk::CompareOp::eAlways, {}, {}, vk::BorderColor::eFloatOpaqueBlack, vk::False
		));
	}
	context.ReleaseTempCmd(cmd);
	for (auto& staging_buffer : staging_buffers) {
		staging_buffer.Destroy(context);
	}
}

void TexturePool::Destroy(const DeviceContext& context) {
	const auto& device = context.GetDevice();
	for (auto& descriptor : texture_array) {
		device.destroyImageView(descriptor.imageView);
		device.destroySampler(descriptor.sampler);
	}
	for (auto& pair : texture_map) {
		pair.second.image.Destroy(context);
	}
}
