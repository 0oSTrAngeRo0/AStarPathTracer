#include "Engine/Resources/ResourceData.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourceRegistry.h"
#include "Engine/Resources/JsonSerializer.h"

template <> const std::string& Resource<ObjResourceData>::GetResourceTypeStatic() {
	static std::string type = "Obj";
	return type;
}
JSON_SERIALIZER(ObjResourceData, <>, path);
REGISTER_RESOURCE_SERIALIZER(ObjResourceData);
REGISTER_RESOURCE_DESERIALIZER(ObjResourceData);


JSON_SERIALIZER(SimpleLitMaterialData, <>, color);
template <> const std::string& Resource<MaterialResourceData<SimpleLitMaterialData>>::GetResourceTypeStatic() {
	static std::string type = "MaterialSimpleLit";
	return type;
}
template <> MaterialResourceData<SimpleLitMaterialData>::MaterialResourceData() :
	material_visitor(ShaderHostBuffer<SimpleLitMaterialData>::GetInstance().CreateVisitor()) {}
JSON_SERIALIZER(MaterialResourceData<TMatData>, <typename TMatData>, material_data);
REGISTER_RESOURCE_SERIALIZER(MaterialResourceData<SimpleLitMaterialData>);
REGISTER_RESOURCE_DESERIALIZER(MaterialResourceData<SimpleLitMaterialData>);
