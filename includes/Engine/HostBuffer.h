#pragma once

#include <vector>
#include <memory>
#include "Engine/Guid.h"


// Todo 限定为值类型
template <typename T>
class HostBuffer {
protected:
	std::vector<T> data;
	bool is_dirty;
	std::unordered_map<Uuid, size_t> handlers;
public:
	const T& GetData(const Uuid& handle) const;
	inline bool IsDirty() const { return is_dirty; }
	inline const std::vector<T>& GetHostBuffer() const { return data; }
	inline const size_t GetIndex(const Uuid& handle) const;

	const Uuid CreateData();
	void SetData(const Uuid& handle, const T& data);
	void ReleaseHandle(const Uuid& handle);
};

template<typename T>
inline const T& HostBuffer<T>::GetData(const Uuid& handle) const {
	if (!handlers.contains(handle)) {
		throw std::runtime_error("Invalid handle: [" + handle.str() + "]");
	}
	return data[handlers.at(handle)];
}

template<typename T>
inline const size_t HostBuffer<T>::GetIndex(const Uuid& handle) const {
	if (!handlers.contains(handle)) {
		throw std::runtime_error("Invalid handle: [" + handle.str() + "]");
	}
	return handlers.at(handle);
}

template<typename T>
inline const Uuid HostBuffer<T>::CreateData() {
	data.emplace_back();
	is_dirty = true;
	Uuid handle = xg::newGuid();
	handlers.emplace(handle, data.size() - 1);
	return handle;
}

template<typename T>
inline void HostBuffer<T>::SetData(const Uuid& handle, const T& data) {
	if (!handlers.contains(handle)) {
		throw std::runtime_error("Invalid handle: [" + handle.str() + "]");
	}
	this->data[handlers.at(handle)] = data;
	is_dirty = true;
}

template<typename T>
inline void HostBuffer<T>::ReleaseHandle(const Uuid& handle) {
	throw std::runtime_error("Unimplemented method");
}
