#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>
#include "Core/Buffer.h"

class DeviceContext;

/// <summary>
/// Todo: Make it more extensible
/// </summary>
class RayTracingShaderBindingTable {
private:
	Buffer buffer;
public:
	vk::StridedDeviceAddressRegionKHR rgen;
	vk::StridedDeviceAddressRegionKHR miss;
	vk::StridedDeviceAddressRegionKHR hit;
	vk::StridedDeviceAddressRegionKHR call;

	RayTracingShaderBindingTable(const DeviceContext& context, vk::Pipeline pipeline);

	void Destroy(const DeviceContext& context);
};