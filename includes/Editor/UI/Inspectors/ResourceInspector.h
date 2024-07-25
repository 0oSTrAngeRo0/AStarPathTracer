#pragma once

#include <memory>

#include "Editor/UI/Inspectors/EditorInspector.h"
#include "Engine/Resources/Resources.h"

class ResourceInspectorFactory {
private:
	using InspectorCreateFunction = std::function<std::unique_ptr<EditorInspectorBase>(ResourceBase&)>;
	static std::unordered_map<MaterialType, InspectorCreateFunction> material_resource_create_functions;
	static std::unordered_map<ResourceType, InspectorCreateFunction> resource_create_functions;
public:
	static std::unique_ptr<EditorInspectorBase> CreateInspector(ResourceBase& resource);
};

template <typename TData>
class ResourceInspector : public EditorInspectorBase {
protected:
	ResourceTemplate<TData>& data;
public:
	ResourceInspector(ResourceTemplate<TData>& data) :data(data) {}
	virtual ~ResourceInspector() = default;
	void DrawInspector() override;
};
