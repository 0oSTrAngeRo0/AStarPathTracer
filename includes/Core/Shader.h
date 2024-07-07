#pragma once

#include "Core/Buffer.h"
#include <glm/glm.hpp>
#include "Core/DataManager.h"

class DeviceContext;

struct LitMaterialData {
	glm::vec4 color;
};

struct ConstantsData {
	glm::mat4 view_inverse;
	glm::mat4 projection_inverse;
};

class Material {
public:
	virtual inline const vk::DeviceAddress GetBufferAddress() const = 0;
	virtual inline const uint16_t GetIndex() const = 0;
};

class ShaderBase {
public:
	virtual void Destroy(const DeviceContext& context) = 0;
	virtual void UpdateData(const DeviceContext& context) = 0;
	virtual inline std::shared_ptr<Material> CreateMaterial() = 0;
	virtual inline vk::DeviceAddress GetBufferAddress() = 0;
};

template <typename TData>
class Shader : public ShaderBase, public DataManager<TData> {
public:
	Shader() :DataManager<TData>(128) {}
	void Destroy(const DeviceContext& context) override { DataManager<TData>::Destroy(context); }
	void UpdateData(const DeviceContext& context) override { 
		DataManager<TData>::UpdateData(context);
		DataManager<TData>::buffer.SetName(context, "Material Buffer");
	}
	inline std::shared_ptr<Material> CreateMaterial() override;
	inline vk::DeviceAddress GetBufferAddress() override { return DataManager<TData>::GetBufferAddress(); }
	inline TData& GetData(size_t index) { return DataManager<TData>::data[index]; }
};

template <typename TData>
class MaterialTemplate : public Material {
public:
	MaterialTemplate(std::shared_ptr<DataHandler<TData>> handler) : handler(handler) {}
	inline TData GetData() const { return handler->GetData(); }
	inline void SetData(const TData& data) { handler->SetData(data); }
	inline const vk::DeviceAddress GetBufferAddress() const override { return handler->GetBufferAddress(); }
	inline const uint16_t GetIndex() const override { return handler->GetIndex(); }
private:
	std::shared_ptr<DataHandler<TData>> handler;
};

template<typename TData>
inline std::shared_ptr<Material> Shader<TData>::CreateMaterial() {
	std::shared_ptr<DataHandler<TData>> data = DataManager<TData>::CreateData();
	std::shared_ptr<MaterialTemplate<TData>> mat_template = std::make_shared<MaterialTemplate<TData>>(data);
	std::shared_ptr<Material> material = std::static_pointer_cast<Material>(mat_template);
	return material;
}
