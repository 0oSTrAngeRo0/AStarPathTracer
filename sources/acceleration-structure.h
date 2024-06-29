#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>

class Mesh;
class DeviceContext;
class Object;
class Buffer;

std::tuple<vk::AccelerationStructureKHR, Buffer, Buffer> CreateAs(
	const DeviceContext& context,
	std::vector<std::shared_ptr<Mesh>>& meshes,
	const std::vector<Object>& objects
);
