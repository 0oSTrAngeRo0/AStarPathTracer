#pragma once

#include "Core/Buffer.h"
#include "Engine/Guid.h"
#include <map>
#include <cstddef>

class DeviceContext;

class MaterialPool {
public:
	const Buffer& GetShaderBuffer(const Uuid& id);
	void EnsureBuffers(const DeviceContext& context, const std::vector<std::tuple<const Uuid, const std::vector<std::byte>, const bool>>& buffers);
	void EnsureMaterials(const std::vector<Uuid>& materials);
	void Destroy(const DeviceContext& context);
	inline const Buffer& GetMainBuffer() const { return main_buffer; }
	
private:
	Buffer main_buffer;
	std::map<Uuid, Buffer> shader_buffers;
};