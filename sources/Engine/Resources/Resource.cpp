#include "Engine/Resources/Resources.h"
#include "Engine/Resources/JsonSerializer.h"
#include "Engine/ShaderHostBuffer.h"
#include "Engine/ShaderHostBuffer.h"

std::string ResourceBase::Serialize(const ResourceBase& data) {
	nlohmann::json j = data;
	return j.dump(4);
}

std::unique_ptr<ResourceBase> ResourceBase::Deserialize(const std::string& str) {
	nlohmann::json j = nlohmann::json::parse(str);
	auto result = nlohmann::adl_serializer<ResourceBase>::from_json(j);
	if (!result.has_value()) {
		throw std::runtime_error("Failed to deserialize: [" + str + "]");
	}
	return std::move(result.value());
}
