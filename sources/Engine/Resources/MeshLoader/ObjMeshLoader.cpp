#define TINYOBJLOADER_IMPLEMENTATION
#include "Engine/Resources/MeshLoader/tiny_obj_loader.h"

#include "Engine/Resources/ResourceData.h"
#include "Engine/Resources/Resources.h"
#include "Engine/Resources/MeshLoader/MikkTSpaceTangentGenerator.h"
#include "Engine/Resources/MeshLoader/MeshResourceUtilities.h"

template <>
MeshData MeshResourceUtilities::Load<ObjResourceData>(const Resource<ObjResourceData>& resource) {
    printf("Start Loading Obj File: [%s]\n", resource.resource_data.path.c_str());
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig reader_config;
    reader_config.vertex_color = false;
    reader_config.triangulate = true;
    if (!reader.ParseFromFile(resource.resource_data.path, reader_config)) {
        throw std::runtime_error("Failed to parse, error: [" + reader.Error() + "]");
    }
    if (!reader.Warning().empty()) {
        printf("Warning when parse: [%s]\n", reader.Warning().c_str());
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<uint32_t> indices;

    assert(attrib.vertices.size() % 3 == 0);
    for (size_t i = 0, end = attrib.vertices.size(); i < end; i += 3) {
        positions.emplace_back(glm::vec3(
            attrib.vertices[i + 0], // vx 
            attrib.vertices[i + 1], // vy
            attrib.vertices[i + 2]  // vz
        ));

        // Todo: Use correct data
        normals.emplace_back(glm::vec3());
        uvs.emplace_back(glm::vec2());
        //normals.emplace_back(glm::vec3(
        //    attrib.normals[i + 0], // nx 
        //    attrib.normals[i + 1], // ny
        //    attrib.normals[i + 2]  // nz
        //));
        //uvs.emplace_back(glm::vec2(
        //    attrib.texcoords[i + 0], // uv_x 
        //    attrib.texcoords[i + 1] // uv_y
        //));
    }
    
    // Loop over shapes
    for (const tinyobj::shape_t& shape : shapes) {
        printf("Loading Shape: [%s]\n", shape.name.c_str());
        if (shape.mesh.indices.size() % 3 != 0)
            throw std::runtime_error("Invalid indices count");
        for (const tinyobj::index_t& index : shape.mesh.indices)
            indices.emplace_back(index.vertex_index);
        printf("\n");
    }

    MeshData mesh;
    mesh.indices = reinterpret_cast<std::vector<glm::uvec3>&>(indices);
    mesh.positions = positions;
    mesh.normals = normals;
    mesh.uvs = uvs;
    mesh.tangents = std::vector<glm::vec4>(positions.size());
    
    //MikkTSpaceTangentGenerator::GenerateTangent(mesh);

    return mesh;
}

REGISTER_RESOURCE_MESH_LOADER(Obj, ObjResourceData);