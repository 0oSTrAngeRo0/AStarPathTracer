#pragma once

#include <memory>

#include "Editor/UI/Inspectors/EditorInspector.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourcesManager.h"
#include "Engine/Resources/ResourceData.h"
#include "Engine/StaticRegistry.h"

class ResourceInspectorFactory {
private:
	using InspectorCreateFunction = std::function<std::unique_ptr<EditorInspectorBase>(ResourceBase&)>;
public:
	static std::unique_ptr<EditorInspectorBase> CreateInspector(ResourceBase& resource);
};

template <typename TData>
class ResourceInspector : public EditorInspectorBase {
protected:
	bool is_dirty;
	Resource<TData>& data;
public:
	ResourceInspector(Resource<TData>& data) :data(data) {}
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
