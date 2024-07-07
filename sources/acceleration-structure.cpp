#include "acceleration-structure.h"
#include "Core/Mesh.h"
#include "algorithm-util.h"
#include "Core/DeviceContext.h"
#include "Core/Buffer.h"

struct BlasInput
{
	std::vector<vk::AccelerationStructureGeometryKHR>       geometry;
	std::vector<vk::AccelerationStructureBuildRangeInfoKHR> build_offset_info;
	vk::BuildAccelerationStructureFlagsKHR                  flags{ 0 };
};

struct BuildAccelerationStructure
{
	vk::AccelerationStructureBuildGeometryInfoKHR build_info;
	std::vector<vk::AccelerationStructureBuildRangeInfoKHR> range_infos;
	vk::AccelerationStructureBuildSizesInfoKHR size_info;
	vk::AccelerationStructureKHR as;
	Buffer as_buffer;
};

BlasInput CreateBlasInput(const DeviceContext& context, const Mesh& mesh)
{
	vk::AccelerationStructureGeometryTrianglesDataKHR triganles(
		vk::Format::eR32G32B32Sfloat, vk::DeviceOrHostAddressConstKHR(mesh.GetVertexAddress()), mesh.GetVertexStride(), mesh.GetVertexCount(),
		vk::IndexType::eUint32, vk::DeviceOrHostAddressConstKHR(mesh.GetIndexAddress()), {});

	vk::AccelerationStructureGeometryKHR geometry(
		vk::GeometryTypeKHR::eTriangles,
		vk::AccelerationStructureGeometryDataKHR(triganles),
		vk::GeometryFlagBitsKHR::eOpaque);

	vk::AccelerationStructureBuildRangeInfoKHR offset(mesh.GetTrianglesCount(), 0, 0, 0);

	BlasInput input = {};
	input.build_offset_info.push_back(offset);
	input.geometry.push_back(geometry);
	return input;
}

void CmdCreateBlas(
	const DeviceContext& context,
	const vk::CommandBuffer cmd,
	BuildAccelerationStructure& build,
	const vk::DeviceAddress scratch_address
) {
	build.as_buffer = Buffer(context, vk::BufferCreateInfo({}, build.size_info.accelerationStructureSize,
		vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR | vk::BufferUsageFlagBits::eShaderDeviceAddress));
	vk::AccelerationStructureCreateInfoKHR create_info({}, build.as_buffer, 0, build.size_info.accelerationStructureSize,
		vk::AccelerationStructureTypeKHR::eBottomLevel, 0);
	build.as = context.GetDevice().createAccelerationStructureKHR(create_info);
	build.build_info.dstAccelerationStructure = build.as;
	build.build_info.scratchData.deviceAddress = scratch_address;
	std::vector<vk::AccelerationStructureBuildRangeInfoKHR*> range_infos;
	for (auto iter = build.range_infos.begin(); iter != build.range_infos.end(); ++iter) {
		range_infos.push_back(iter._Unwrapped());
	}
	cmd.buildAccelerationStructuresKHR(build.build_info, range_infos);
	std::vector<vk::MemoryBarrier> barriers = {
		vk::MemoryBarrier(vk::AccessFlagBits::eAccelerationStructureWriteKHR, vk::AccessFlagBits::eAccelerationStructureReadKHR)
	};
	cmd.pipelineBarrier(vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR, vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
		{}, barriers, {}, {});
}

std::vector<BuildAccelerationStructure> BuildBlas(
	const DeviceContext& context,
	const std::vector<BlasInput>& inputs,
	vk::BuildAccelerationStructureFlagsKHR flags
) {
	vk::DeviceSize max_scratch_size = 0;
	std::vector<BuildAccelerationStructure> build_as;
	for (uint32_t i = 0, end = inputs.size(); i < end; i++)
	{
		const BlasInput& input = inputs[i];
		BuildAccelerationStructure build = {};

		build.build_info = vk::AccelerationStructureBuildGeometryInfoKHR(
			vk::AccelerationStructureTypeKHR::eBottomLevel, input.flags | flags,
			vk::BuildAccelerationStructureModeKHR::eBuild, {}, {}, input.geometry);

		build.range_infos = input.build_offset_info;

		std::vector<uint32_t> max_primitive_counts = Select<vk::AccelerationStructureBuildRangeInfoKHR, uint32_t>(input.build_offset_info,
			[](const vk::AccelerationStructureBuildRangeInfoKHR& offset) -> uint32_t {return offset.primitiveCount; });

		build.size_info = context.GetDevice().getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice,
			build.build_info, max_primitive_counts);
		max_scratch_size = std::max(max_scratch_size, build.size_info.buildScratchSize);
		build_as.emplace_back(build);
	}

	Buffer scratch_buffer(context, vk::BufferCreateInfo({}, max_scratch_size,
		vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eStorageBuffer));
	scratch_buffer.SetName(context, "Blas Scratch Buffer");

	// 分块避免单次Command处理过多数据
	std::vector<uint32_t> indices;
	vk::DeviceSize batch_size = 0;
	vk::DeviceSize batch_limit = 256000000; // 256 MB
	for (uint32_t i = 0, end = inputs.size(); i < end; i++)
	{
		indices.emplace_back(i);
		batch_size += build_as[i].size_info.accelerationStructureSize;
		if (batch_size < batch_limit && i != end - 1) continue;
		vk::CommandBuffer cmd = context.GetTempCmd();
		for (uint32_t index : indices) {
			CmdCreateBlas(context, cmd, build_as[index], scratch_buffer.GetDeviceAddress());
		}
		context.ReleaseTempCmd(cmd);
		indices.clear();
		batch_size = 0;
	}
	scratch_buffer.Destroy(context);
	return build_as;
}

std::tuple<vk::AccelerationStructureKHR, Buffer, Buffer> BuildTlas(
	const DeviceContext& context,
	const std::vector<vk::AccelerationStructureInstanceKHR>& instances,
	vk::BuildAccelerationStructureFlagsKHR flags, bool update
) {
	vk::BufferCreateInfo instance_buffer_ci({}, {},
		vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eAccelerationStructureBuildInputReadOnlyKHR);
	Buffer instances_buffer = Buffer::CreateWithData<vk::AccelerationStructureInstanceKHR>(context, instance_buffer_ci, instances);

	auto cmd = context.GetTempCmd();

	std::vector<vk::MemoryBarrier> barriers = {
		vk::MemoryBarrier(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eAccelerationStructureWriteKHR)
	};
	cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR, {}, barriers, {}, {});

	std::vector<vk::AccelerationStructureGeometryKHR> geometries = {
		vk::AccelerationStructureGeometryKHR(
			vk::GeometryTypeKHR::eInstances,
			vk::AccelerationStructureGeometryInstancesDataKHR({}, instances_buffer.GetDeviceAddress())
		),
	};

	vk::AccelerationStructureBuildGeometryInfoKHR build_info(vk::AccelerationStructureTypeKHR::eTopLevel, flags,
		update ? vk::BuildAccelerationStructureModeKHR::eUpdate : vk::BuildAccelerationStructureModeKHR::eBuild, {}, {}, geometries);

	std::vector<uint32_t> max_primitive_counts = {
		static_cast<uint32_t>(instances.size())
	};

	vk::AccelerationStructureBuildSizesInfoKHR size_info = context.GetDevice().getAccelerationStructureBuildSizesKHR(
		vk::AccelerationStructureBuildTypeKHR::eDevice, build_info, max_primitive_counts);

	Buffer tlas_buffer(context, vk::BufferCreateInfo({}, size_info.accelerationStructureSize,
		vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR | vk::BufferUsageFlagBits::eShaderDeviceAddress));
	vk::AccelerationStructureCreateInfoKHR create_info({}, tlas_buffer, 0, size_info.accelerationStructureSize,
		vk::AccelerationStructureTypeKHR::eTopLevel);
	vk::AccelerationStructureKHR tlas = context.GetDevice().createAccelerationStructureKHR(create_info);

	Buffer scratch_buffer(context, vk::BufferCreateInfo({}, size_info.buildScratchSize,
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress));
	scratch_buffer.SetName(context, "Tlas Scratch Buffer");

	build_info.dstAccelerationStructure = tlas;
	build_info.scratchData = scratch_buffer.GetDeviceAddress();

	vk::AccelerationStructureBuildRangeInfoKHR range_info(instances.size());
	std::vector<vk::AccelerationStructureBuildGeometryInfoKHR> build_infos = {
		build_info
	};
	std::vector<vk::AccelerationStructureBuildRangeInfoKHR*> range_infos = {
		&range_info
	};
	cmd.buildAccelerationStructuresKHR(build_infos, range_infos);
	context.ReleaseTempCmd(cmd);
	scratch_buffer.Destroy(context);

	tlas_buffer.SetName(context, "Tlas Buffer");
	return std::tie(tlas, tlas_buffer, instances_buffer);
}

void CreateBlases(const DeviceContext& context, std::vector<std::shared_ptr<Mesh>>& meshes) {
	std::vector<BlasInput> inputs;
	for (const auto& mesh : meshes) {
		inputs.emplace_back(CreateBlasInput(context, *mesh));
	}
	auto blases = BuildBlas(context, inputs, {});
	assert(meshes.size() == blases.size());
	for (size_t i = 0, end = meshes.size(); i < end; i++) {
		blases[i].as_buffer.SetName(context, "Blas");
		meshes[i]->SetAsData(blases[i].as, blases[i].as_buffer);
	}
}
