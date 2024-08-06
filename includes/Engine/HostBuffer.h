#pragma once

#include <vector>
#include <memory>

template <typename T> class HostBufferVisitor;

// Todo 限定为值类型
template <typename T>
class HostBuffer {
protected:
	std::vector<T> data;
	bool is_dirty;
	std::vector<std::shared_ptr<HostBufferVisitor<T>>> visitors;
public:
	T& GetData(const HostBufferVisitor<T>& visitor);
	std::shared_ptr<HostBufferVisitor<T>> CreateVisitor();
	void SetData(const HostBufferVisitor<T>& visitor, const T& data);
	void ReleaseVisitor(std::shared_ptr<HostBufferVisitor<T>> visitor);
	friend class HostBufferVisitor<T>;
};

template <typename T>
class HostBufferVisitor {
protected:
	HostBuffer<T>& owner;
	uint32_t index;
public:
	HostBufferVisitor(HostBuffer<T>& owner, uint32_t index) :owner(owner), index(index) {}
	inline T& GetData() { return owner.GetData(*this); }
	inline void SetData(const T& data) { return owner.SetData(*this, data); }
	friend class HostBuffer<T>;
};

template<typename T>
inline T& HostBuffer<T>::GetData(const HostBufferVisitor<T>& visitor) {
	return data[visitor.index];
}

template<typename T>
inline std::shared_ptr<HostBufferVisitor<T>> HostBuffer<T>::CreateVisitor() {
	data.emplace_back();
	is_dirty = true;
	auto visitor = std::make_shared<HostBufferVisitor<T>>(*this, data.size() - 1);
	visitors.emplace_back(visitor);
	return visitor;
}

template<typename T>
inline void HostBuffer<T>::SetData(const HostBufferVisitor<T>& visitor, const T& data) {
	this->data[visitor.index] = data;
	is_dirty = true;
}

template<typename T>
inline void HostBuffer<T>::ReleaseVisitor(std::shared_ptr<HostBufferVisitor<T>> visitor) {}
