#include "Core/ModelLoader/ObjLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION

#include "Core/ModelLoader/tiny_obj_loader.h"

std::tuple<std::vector<glm::vec3>, std::vector<uint32_t>> LoadFromFile(const std::string& path) {
    printf("Start Loading Obj File: [%s]\n", path.c_str());
	tinyobj::ObjReader reader;
	tinyobj::ObjReaderConfig reader_config;
	reader_config.vertex_color = false;
    reader_config.triangulate = true;
	if (!reader.ParseFromFile(path, reader_config)) {
		throw std::runtime_error("Failed to parse, error: [" + reader.Error() + "]");
	}
	if (!reader.Warning().empty()) {
		printf("Warning when parse: [%s]\n", reader.Warning().c_str());
	}

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;

    if (attrib.vertices.size() % 3 != 0)
        throw std::runtime_error("Invalid vertex count");
    for (size_t i = 0, end = attrib.vertices.size(); i < end; i+=3)
    {
        vertices.emplace_back(
            glm::vec3(
                attrib.vertices[i + 0], // vx 
                attrib.vertices[i + 1], // vy
                attrib.vertices[i + 2]  // vz
            )
        );
    }

    // Loop over shapes
    for (const tinyobj::shape_t& shape: shapes) {
        printf("Loading Shape: [%s]\n", shape.name.c_str());
        if (shape.mesh.indices.size() % 3 != 0)
            throw std::runtime_error("Invalid indices count");
        for (const tinyobj::index_t& index: shape.mesh.indices)
            indices.emplace_back(index.vertex_index);
        printf("\n");
    }

    return std::tie(vertices, indices);
}