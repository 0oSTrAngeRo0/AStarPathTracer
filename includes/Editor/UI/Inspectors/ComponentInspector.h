#pragma once

#include <string>
#include <entt/entt.hpp>
#include "Editor/UI/Inspectors/EditorInspector.h"
#include "Utilities/ReflectionX.h"
#include "Utilities/EnumX.h"

class ComponentInspectorBase : public EditorInspectorBase {
public:
	virtual void DrawInspector() = 0;
	virtual const std::string& GetComponentName() const = 0;
	virtual ~ComponentInspectorBase() = default;
};

class UnknownComponentInspector : public ComponentInspectorBase {
public:
	UnknownComponentInspector(const entt::type_info& type_info);
	void DrawInspector() override;
	const std::string& GetComponentName() const override { return name; }
private:
	std::string name;
};

template <typename TComponent>
class ComponentInspector : public ComponentInspectorBase {
public:
	ComponentInspector(const entt::type_info& type_info, entt::registry& registry, entt::entity entity) :
		entity(entity),
		name(reflection::GetTypeName(type_info.hash()).value_or(std::string(type_info.name()))),
		component(registry.get<TComponent>(entity)) {}
	virtual void DrawInspector() override {}
	const std::string& GetComponentName() const override { return name; }
protected:
	entt::entity entity;
	std::string name;
	TComponent& component;

	std::string CatId(std::string&& str) { return str + "##" + std::to_string(GetEnumUnderlying(entity)); }
};

class EntityInspector : public EditorInspectorBase {
public:
	EntityInspector(entt::entity entity, entt::registry& registry);
	void DrawInspector() override;
	~EntityInspector();
private:
	std::vector<std::unique_ptr<ComponentInspectorBase>> components;
};


namespace reflection {
	template <typename TInspector>
	concept ConceptComponentInspector = 
		std::is_constructible_v<TInspector, 
			const entt::type_info&, 
			entt::registry&, 
			entt::entity
		> && 
		std::is_base_of_v<ComponentInspectorBase, TInspector>;
	
	template <ConceptComponentInspector TInspector>
	inline std::unique_ptr<ComponentInspectorBase> ConstructComponentInspector(
		const entt::type_info& type_id,
		entt::registry& registry,
		entt::entity entity
	) {
		return std::make_unique<TInspector>(type_id, registry, entity);
	}

	template <typename TComponent, ConceptComponentInspector TInspector>
	inline void RegisterComponentInspector() {
		entt::meta<TComponent>().func<&ConstructComponentInspector<TInspector>>(entt::hashed_string("CreateComponentInspector"));
	}

	inline std::optional<std::unique_ptr<ComponentInspectorBase>> CreateComponentInspector(
		const entt::type_info& type_info,
		entt::registry& registry, 
		entt::entity entity
	) {
		auto constructor = entt::resolve(type_info.hash()).func(entt::hashed_string("CreateComponentInspector"));
		if (!constructor) return std::nullopt;
		auto any = constructor.invoke({}, entt::forward_as_meta(type_info), entt::forward_as_meta(registry), entt::forward_as_meta(entity));
		return any.cast<std::unique_ptr<ComponentInspectorBase>&&>();
	}
}
