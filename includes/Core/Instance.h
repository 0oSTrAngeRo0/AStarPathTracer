#pragma once

#include "Core/DataManager.h"

struct InstanceData {
	vk::DeviceAddress vertex_address;
	vk::DeviceAddress index_address;
	vk::DeviceAddress material_address;
	uint16_t material_index;

	InstanceData() {}

	InstanceData(vk::DeviceAddress vertex, vk::DeviceAddress index, vk::DeviceAddress material, uint16_t material_index) :
		vertex_address(vertex), index_address(index), material_address(material), material_index(material_index) {}
};

class InstanceHandler : public DataHandler<InstanceData> {};

class InstancesManager : public DataManager<InstanceData> {
public:
	InstancesManager() :DataManager<InstanceData>(128) {}
	inline std::shared_ptr<InstanceHandler> CreateInstance() {
		return std::static_pointer_cast<InstanceHandler>(DataManager<InstanceData>::CreateData());
	}
};
