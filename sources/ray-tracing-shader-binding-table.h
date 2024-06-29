#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>
#include "Core/Buffer.h"

class DeviceContext;

class RayTracingBindingTable {
private:
	Buffer buffer;
public:
	vk::StridedDeviceAddressRegionKHR rgen;
	vk::StridedDeviceAddressRegionKHR miss;
	vk::StridedDeviceAddressRegionKHR hit;
	vk::StridedDeviceAddressRegionKHR call;

	RayTracingBindingTable(const DeviceContext& context, vk::Pipeline pipeline);

	void Destroy(const DeviceContext& context);
};