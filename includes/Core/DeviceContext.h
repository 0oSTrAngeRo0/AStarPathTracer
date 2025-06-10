#pragma once

#include "Core/VulkanUsages.h"

#define ENABLE_DEBUG 1
//#define ENABLE_NSIGHT_AFTERMATH 1

#if ENABLE_DEBUG
class EngineDebugger;
#endif

#include "TemporaryCommandBufferPool.h"

class VulkanWindow;

class DeviceContextCreateConfig {
public:
	virtual std::vector<std::string> GetInstanceExtensions() const = 0;
	virtual std::vector<std::string> GetInstanceLayers() const = 0;
	virtual std::vector<std::string> GetGpuExtensions() const = 0;
	virtual bool OtherGpuCheck(vk::PhysicalDevice gpu) const = 0;
	virtual vk::PhysicalDeviceFeatures2 GetGpuFeatures() const = 0;
};

class DeviceContext {
public:
	DeviceContext(DeviceContextCreateConfig& config);
	~DeviceContext();
	inline vk::Instance GetInstance() const { return instance; }
	inline vk::Device GetDevice() const { return device; }
	inline vk::PhysicalDevice GetGpu() const { return gpu; }
	inline vk::Queue GetGraphicsQueue() const { return graphics_queue; }
	inline vk::Queue GetPresentQueue() const { return present_queue; }
	inline uint32_t GetGrpahicsQueueIndex() const { return graphics_queue_index; }
	inline uint32_t GetPresentQueueIndex() const { return present_queue_index; }
	inline vma::Allocator GetAllocator() const { return allocator; }
	inline const vk::CommandBuffer GetTempCmd() const { return temp_cmd_pool->Get(device); }
	inline void ReleaseTempCmd(const vk::CommandBuffer buffer) const { temp_cmd_pool->Release(buffer, device, graphics_queue); }
	template <typename TFeature> inline const TFeature GetGpuFeature() const { 
		if constexpr (std::is_same_v<TFeature, vk::PhysicalDeviceFeatures2>) {
			return gpu.getFeatures2();
		}
		else {
			return gpu.getFeatures2<vk::PhysicalDeviceFeatures2, TFeature>().get<TFeature>();
		}
	}
	template <typename TProperty> inline const TProperty GetGpuProperty() const { 
		if constexpr (std::is_same_v<TProperty, vk::PhysicalDeviceProperties2>) {
			return gpu.getProperties2();
		}
		else {
			return gpu.getProperties2<vk::PhysicalDeviceProperties2, TProperty>().get<TProperty>();
		}
	}

private:
	struct QueueIndices {
		uint32_t graphics;
		uint32_t present;
	};

	vk::Instance instance;
	vk::Device device;

	vk::PhysicalDevice gpu;
	uint32_t graphics_queue_index;
	uint32_t present_queue_index;

	vk::Queue graphics_queue;
	vk::Queue present_queue;

	std::unique_ptr<TemporaryCommandBufferPool> temp_cmd_pool;

	#if ENABLE_DEBUG
	std::unique_ptr<EngineDebugger> debugger;
	#endif

	vma::Allocator allocator;

	static vk::Instance CreateInstance(const DeviceContextCreateConfig& config);
	static std::optional<QueueIndices> GetSuitableQueueFamilies(vk::PhysicalDevice gpu);
	static std::optional<vk::PhysicalDevice> PickPhysicalDevice(vk::Instance instance, const DeviceContextCreateConfig& config);
	static bool IsGpuExtensionsValid(vk::PhysicalDevice gpu, std::vector<std::string> extensions);
	static vk::Device CreateDevice(const vk::PhysicalDevice gpu, const DeviceContextCreateConfig& config, const QueueIndices& queue_indices);
	static vma::Allocator CreateVmaAllocator(const vk::Instance instance, const vk::PhysicalDevice gpu, const vk::Device device);
};