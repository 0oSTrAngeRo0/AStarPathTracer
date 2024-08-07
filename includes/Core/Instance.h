#pragma once

#include "Core/DataManager.h"

struct InstanceData {
	vk::DeviceAddress vertex_position_address;
	vk::DeviceAddress vertex_other_address;
	vk::DeviceAddress index_address;
	vk::DeviceAddress material_address;
	uint16_t material_index;

	InstanceData(
		vk::DeviceAddress vertex_position_address, 
		vk::DeviceAddress vertex_other_address, 
		vk::DeviceAddress index, 
		vk::DeviceAddress material_address, 
		uint16_t material_index
	) : vertex_position_address(vertex_position_address), 
		vertex_other_address(vertex_other_address),
		index_address(index), 
		material_address(material_address), 
		material_index(material_index) {}
};
