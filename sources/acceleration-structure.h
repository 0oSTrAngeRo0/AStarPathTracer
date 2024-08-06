#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>

class Mesh;
class DeviceContext;
class Object;
class Buffer;

std::tuple<vk::AccelerationStructureKHR, Buffer, Buffer> BuildTlas(
	const DeviceContext& context,
	const std::vector<vk::AccelerationStructureInstanceKHR>& instances,
	vk::BuildAccelerationStructureFlagsKHR flags, bool update
);

void CreateBlases(const DeviceContext& context, std::vector<std::reference_wrapper<Mesh>>& meshes);