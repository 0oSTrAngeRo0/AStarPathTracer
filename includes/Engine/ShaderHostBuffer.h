#pragma once

#include "Engine/HostBuffer.h"

class HostShader {
public:
	HostShader(const Uuid& id, const size_t stride) : id(id), stride(stride), memory(), is_dirty(false) {
		ASTAR_ASSERT(stride != 0);
	}

	inline size_t GetStride() const { return stride; }
	inline Uuid GetId() const { return id; }
	inline std::vector<std::byte> GetData() const { return memory.GetData<std::byte>(); }
	inline bool IsDirty() const { return is_dirty; }

	template <typename T> inline const T GetValue(const Uuid& handle) const {
		AssertStride<T>();
		return memory.Get<T>(GetIndex(handle));
	}
	inline size_t GetIndex(const Uuid& handle) const { 
		ASTAR_ASSERT(handlers.contains(handle));
		return handlers.at(handle);
	}
	template <typename T> inline size_t EmplaceValue(const Uuid& handle, const T value) {
		AssertStride<T>();
		size_t offset = memory.EmplaceBack<T>(value);
		ASTAR_ASSERT(offset % stride == 0);
		size_t index = offset / stride;
		handlers.insert(std::make_pair(handle, index));
		is_dirty = true;
		return index;
	}
	template <typename T> inline void SetValue(const Uuid& handle, const T value) {
		AssertStride<T>();
		memory.Get<std::decay_t<T>&>(GetIndex(handle)) = value;
		is_dirty = true;
	}
	void ReleaseHandle(const Uuid& handle) { handlers.erase(handle); }
protected:
	MemoryBlock memory;
	Uuid id;
	size_t stride;
	bool is_dirty;
	std::unordered_map<Uuid, size_t> handlers;
	template <typename T> inline void AssertStride() {
		ASTAR_ASSERT(sizeof(std::decay_t<T>) == stride);
	}
};


