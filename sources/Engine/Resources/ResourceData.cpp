#include "Engine/Resources/ResourceData.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/ResourceRegistry.h"
#include "Engine/Resources/JsonSerializer.h"
#include "Engine/ShaderHostBuffer.h"

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
	runtime_host_id(HostShaderBuffer<SimpleLitMaterialData>::GetInstance().CreateData()) {}
template<typename T> const Uuid MaterialResourceData<T>::GetShaderId() const { return HostShaderBuffer<T>::GetIdStatic(); }
template<typename T> const size_t MaterialResourceData<T>::GetMaterialIndex() const { return HostShaderBuffer<T>::GetInstance().GetMaterialIndex(runtime_host_id); }
JSON_SERIALIZER(MaterialResourceData<TMatData>, <typename TMatData>, material_data);
REGISTER_RESOURCE_SERIALIZER(MaterialResourceData<SimpleLitMaterialData>);
REGISTER_RESOURCE_DESERIALIZER(MaterialResourceData<SimpleLitMaterialData>);
REGISTER_MATERIAL_RUNTIME_DATA(MaterialResourceData<SimpleLitMaterialData>);
