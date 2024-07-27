#include "Engine/Resources/ResourceData.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourceRegistry.h"
#include "Engine/Resources/JsonSerializer.h"

template <> MaterialResourceData<SimpleLitMaterialData>::MaterialResourceData() :
	material_visitor(ShaderHostBuffer<SimpleLitMaterialData>::GetInstance().CreateVisitor()) {}

template <> Resource<ObjResourceData>::Resource() : ResourceBase("Obj") {}

template <> Resource<MaterialResourceData<SimpleLitMaterialData>>::Resource() : ResourceBase("MaterialSimpleLit") {}

JSON_SERIALIZER(ObjResourceData, <>, path);
JSON_SERIALIZER(MaterialResourceData<TMatData>, <typename TMatData>, material_data);

REGISTER_RESOURCE_SERIALIZER(Obj, ObjResourceData);
REGISTER_RESOURCE_SERIALIZER(MaterialSimpleLit, MaterialResourceData<SimpleLitMaterialData>);

REGISTER_RESOURCE_DESERIALIZER(Obj, ObjResourceData);
REGISTER_RESOURCE_DESERIALIZER(MaterialSimpleLit, MaterialResourceData<SimpleLitMaterialData>);
