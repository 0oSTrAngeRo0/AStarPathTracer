#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>
#include "Core/Buffer.h"

class DeviceContext;

class RayTracingShaders {
private:
	static inline uint32_t AlignUp(uint32_t size, uint32_t alignment) {
		return (size + (alignment - 1)) & (~(alignment - 1));
	}

public:

	template <typename T>
	class DataTemplate {
	public:
		DataTemplate() : rgen(), miss(), hit(), call() {}
		inline const T GetRayGen() const { return rgen; }
		inline const T GetMiss() const { return miss; }
		inline const T GetClosestHit() const { return hit; }
		inline const T GetCallable() const { return call; }
		virtual ~DataTemplate() = default;
	protected:
		T rgen;
		T miss;
		T hit;
		T call;
	};

	class PipelineData;
	class ShaderCount : public DataTemplate<uint32_t> {
	public:
		friend class PipelineData;
	};

	class BindingTable : public DataTemplate<vk::StridedDeviceAddressRegionKHR> {
	public:
		BindingTable(const DeviceContext& context, const vk::Pipeline pipeline, const ShaderCount& sizes);
		inline const Buffer& GetBuffer() const { return buffer; }
		inline void Destroy(const DeviceContext& context) { buffer.Destroy(context); }
	protected:
		Buffer buffer;
	};

	class ShaderData {
	public:
		std::vector<std::byte> binary_code;
		vk::ShaderStageFlagBits stage;
		std::string entry_function;
		ShaderData() {}
		ShaderData(const ShaderData& other) : ShaderData(other.binary_code, other.entry_function, other.stage) {}
		ShaderData(const std::vector<std::byte> binary_code, const std::string& entry, const vk::ShaderStageFlagBits stage) :
			binary_code(binary_code), entry_function(entry), stage(stage) {}
	};

	class PipelineData {
	public:
		PipelineData(const DeviceContext& context, const std::vector<ShaderData>& shaders);
		inline const std::vector<vk::PipelineShaderStageCreateInfo>& GetStages() const { return stages; }
		inline const std::vector<vk::RayTracingShaderGroupCreateInfoKHR>& GetGroups() const { return groups; }
		inline const ShaderCount& GetShaderCount() const { return count; }
		void Destroy(const DeviceContext& context);
	protected:
		std::vector<vk::PipelineShaderStageCreateInfo> stages;
		std::vector<vk::RayTracingShaderGroupCreateInfoKHR> groups;
		ShaderCount count;
		std::vector<vk::ShaderModule> modules;
		void IncreaseCount(const vk::ShaderStageFlagBits stage);
	};

	static inline int GetStagePeriority(vk::ShaderStageFlagBits stage) {
		if (stage == vk::ShaderStageFlagBits::eRaygenKHR) {
			return 1;
		}
		else if (stage == vk::ShaderStageFlagBits::eClosestHitKHR) {
			return 3;
		}
		else if (stage == vk::ShaderStageFlagBits::eMissKHR) {
			return 2;
		}
		else if (stage == vk::ShaderStageFlagBits::eCallableKHR) {
			return 4;
		}
		else return -1;
	}
	static inline int StageComparer(const ShaderData& a, const ShaderData& b) {
		return GetStagePeriority(a.stage) < GetStagePeriority(b.stage);
	}
};