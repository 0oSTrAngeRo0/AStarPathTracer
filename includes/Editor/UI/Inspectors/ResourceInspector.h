#pragma once

#include <memory>

#include "Editor/UI/Inspectors/EditorInspector.h"

class ResourceBase;

class ResourceInspectorFactory {
public:
	static std::unique_ptr<EditorInspectorBase> CreateInspector(ResourceBase& resource);
};

template <typename TData>
class ResourceInspector {
protected:
	ResourceTemplate<TData>& data;
public:
	ResourceInspector(ResourceTemplate<TData>& data) :data(data) {}
	virtual ~ResourceInspector() = default;
};

class ObjResourceInspector : public EditorInspectorBase, public ResourceInspector<ObjResourceData> {
public:
	ObjResourceInspector(ResourceTemplate<ObjResourceData>& data) :ResourceInspector<ObjResourceData>::ResourceInspector(data){}
	void DrawInspector() override;
};