#pragma once

#include "Core/VulkanUsages.h"

#define ENABLE_DEBUG 1
#define GPU_FEATURE \
	vk::PhysicalDeviceFeatures2, \
	vk::PhysicalDeviceAccelerationStructureFeaturesKHR, \
	vk::PhysicalDeviceRayTracingPipelineFeaturesKHR, \
	vk::PhysicalDevice16BitStorageFeaturesKHR
#define GPU_PROPERTY \
	vk::PhysicalDeviceProperties2, \
	vk::PhysicalDeviceRayTracingPipelinePropertiesKHR
	

#if ENABLE_DEBUG
class EngineDebugger;
#endif

#include "TemporaryCommandBufferPool.h"

class Window;

class DeviceContext {
private:
	vk::Instance instance;
	vk::Device device;

	vk::SurfaceKHR surface;
	vk::Extent2D extent;
	vk::SurfaceCapabilitiesKHR surface_capabilities;
	vk::PresentModeKHR surface_present_modes;
	vk::SurfaceFormatKHR surface_format;

	vk::PhysicalDevice gpu;
	uint32_t graphics_queue_index;
	uint32_t present_queue_index;
	vk::StructureChain<GPU_FEATURE> gpu_features;
	vk::StructureChain<GPU_PROPERTY> gpu_properties;

	vk::Queue graphics_queue;
	vk::Queue present_queue;

	std::unique_ptr<TemporaryCommandBufferPool> temp_cmd_pool;

#if ENABLE_DEBUG
	std::unique_ptr<EngineDebugger> debugger;
#endif

	vma::Allocator allocator;


public:
	DeviceContext(const Window& window);
	~DeviceContext();
	inline vk::Instance GetInstance() const { return instance; }
	inline vk::Device GetDevice() const { return device; }
	inline vk::PhysicalDevice GetGpu() const { return gpu; }
	inline vk::Queue GetGraphicsQueue() const { return graphics_queue; }
	inline vk::Queue GetPresentQueue() const { return present_queue; }
	inline uint32_t GetGrpahicsQueueIndex() const { return graphics_queue_index; }
	inline uint32_t GetPresentQueueIndex() const { return present_queue_index; }
	inline vk::Extent2D GetActuralExtent() const { return extent; }
	inline vk::SurfaceKHR GetSurface() const { return surface; }
	inline vk::SurfaceCapabilitiesKHR GetSurfaceCapabilities() const { return surface_capabilities; }
	inline vk::PresentModeKHR GetSurfacePresentModes() const { return surface_present_modes; }
	inline vk::SurfaceFormatKHR GetSurfaceFormat() const { return surface_format; }
	inline vma::Allocator GetAllocator() const { return allocator; }
	inline const vk::CommandBuffer GetTempCmd() const { return temp_cmd_pool->Get(device); }
	inline void ReleaseTempCmd(const vk::CommandBuffer buffer) const { temp_cmd_pool->Release(buffer, device, graphics_queue); }
	
	template <typename TFeature>
	inline const TFeature& GetGpuFeature() const {return gpu_features.get<TFeature>(); }

	template <typename TProperty>
	inline const TProperty& GetGpuProperty() const { return gpu_properties.get<TProperty>(); }
};