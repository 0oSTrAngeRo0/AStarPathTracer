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

    // Loop over shapes
    for (const auto& shape : shapes) {
        printf("Loading Shape: [%s]\n", shape.name.c_str());
        size_t offset = 0;
        for (size_t n = 0; n < shape.mesh.num_face_vertices.size(); n++) {
            // per face
            auto ngon = shape.mesh.num_face_vertices[n];
            assert(ngon == 3);
            auto material_id = shape.mesh.material_ids[n];
            for (size_t f = 0; f < ngon; f++) {
                const auto& index = shape.mesh.indices[offset + f];
                positions.emplace_back(glm::vec3(
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                ));

                uvs.emplace_back(glm::vec2(
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                ));

                normals.emplace_back(glm::vec3(
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                ));
                indices.emplace_back(positions.size() - 1);
            }
            offset += ngon;
        }
        printf("\n");
    }

    assert(indices.size() % 3 == 0);
    MeshData mesh;
    mesh.indices = reinterpret_cast<std::vector<glm::uvec3>&>(indices);
    mesh.positions = positions;
    mesh.normals = normals;
    mesh.uvs = uvs;
    mesh.tangents = std::vector<glm::vec4>(positions.size());
    
    MikkTSpaceTangentGenerator::GenerateTangent(mesh);

    return mesh;
}

REGISTER_RESOURCE_MESH_LOADER(ObjResourceData);