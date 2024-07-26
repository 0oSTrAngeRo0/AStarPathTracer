#pragma once

#include <memory>

#include "Editor/UI/Inspectors/EditorInspector.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourcesManager.h"

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
	bool is_dirty;
	ResourceTemplate<TData>& data;
public:
	ResourceInspector(ResourceTemplate<TData>& data) :data(data) {}
	virtual ~ResourceInspector() = default;
	void DrawInspector() override {
		is_dirty = false;
		Draw();
		if (is_dirty) {
			ResourcesManager::GetInstance().SaveResource(data.uuid);
		}
	};
	void Draw();
};
