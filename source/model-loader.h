#ifndef PATHTRACER_SOURCE_MODEL_LOADER_H_
#define PATHTRACER_SOURCE_MODEL_LOADER_H_

#include "glm/glm.hpp"
#include <vector>

namespace asset {
	struct Vertex {
		glm::vec3 position;
		glm::vec2 uv;
	};

	struct Mesh {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};

	Mesh LoadModelObj(const char* path, float additional_scale);
}

#endif //PATHTRACER_SOURCE_MODEL_LOADER_H_
