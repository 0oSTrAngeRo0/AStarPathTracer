#pragma once

import Core;

class DeviceContext;
template <typename TData> class DataHandler;

template <typename TData>
class DataManager {
public:
	DataManager(uint16_t batch_size) :batch_size(batch_size), count(0), max_count(0), is_data_dirty(false) {}
	std::shared_ptr<DataHandler<TData>> CreateData();
	virtual void UpdateData(const DeviceContext& context);
	void Destroy(const DeviceContext& context);
	inline TData GetData(const uint32_t index) const { return data[index]; }
	inline void SetData(const uint32_t index, const TData& data) { this->data[index] = data; is_data_dirty = true; }
	inline uint32_t GetDataCount() const { return count; }
	inline vk::DeviceAddress GetBufferAddress() const { return buffer.GetDeviceAddress(); }
	inline const Buffer& GetBuffer() const { return buffer; }
protected:
	Buffer buffer;
	std::vector<TData> data;
	uint16_t batch_size;
	std::vector<std::shared_ptr<DataHandler<TData>>> handlers;
	uint32_t count;
	uint32_t max_count;
	bool is_data_dirty; // check if buffer need to update
};

template <typename TData>
class DataHandler {
public:
	DataHandler(uint32_t index, DataManager<TData>& manager) : index(index), manager(manager) {}
	inline vk::DeviceAddress GetBufferAddress() const { return manager.GetBufferAddress(); }
	inline TData GetData() const { return manager.GetData(index); }
	inline uint32_t GetIndex() const { return index; }
	inline void SetData(const TData& data) { manager.SetData(index, data); }
protected:
	uint32_t index;
	DataManager<TData>& manager;
};

template<typename TData>
inline std::shared_ptr<DataHandler<TData>> DataManager<TData>::CreateData() {
	count++;
	data.emplace_back(TData());
	std::shared_ptr<DataHandler<TData>> handler = std::make_shared<DataHandler<TData>>(count - 1, *this);
	handlers.emplace_back(handler);
	is_data_dirty = true;
	return handler;
}

template<typename TData>
inline void DataManager<TData>::UpdateData(const DeviceContext& context) {
	if (!is_data_dirty) return;
	if (count > max_count) {
		// Todo: Recreate Buffer with (batch_size * n) larger
		buffer.Destroy(context);
		uint32_t batch_count = (count + batch_size - 1) / batch_size; // batch_size*(batch_count-1) < count <= batch_size*batch_count
		max_count = batch_size * batch_count;
		vk::BufferCreateInfo create_info({}, sizeof(TData) * max_count, vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress);
		vma::AllocationCreateInfo allocation_info(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite, vma::MemoryUsage::eAuto);
		buffer = Buffer(context, create_info, allocation_info);
	}
	buffer.SetData<TData>(context, data);
	is_data_dirty = false;
}

template<typename TData>
inline void DataManager<TData>::Destroy(const DeviceContext& context) {
	buffer.Destroy(context);
}
