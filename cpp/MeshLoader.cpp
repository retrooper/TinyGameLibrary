#include "MeshLoader.h"

namespace tgl {
     Mesh tgl::MeshLoader::loadObj(const char *filePath) {
         return loadObj(filePath, {0.5, 0.5, 0.5, 1});
    }

    Mesh tgl::MeshLoader::loadObj(const char *filePath, glm::vec4 color) {
        Mesh resultMesh;
        tinyobj::attrib_t vertexAttributes;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string errorStr;
        std::string warnStr;
        bool half_loaded = tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, &warnStr, &errorStr, filePath);
        if (!half_loaded) {
            ERROR("Failed to load a model!");
        }

        std::unordered_map<Vertex, uint32_t> newVertices;

        for (const tinyobj::shape_t &shape : shapes) {
            for (tinyobj::index_t index : shape.mesh.indices) {
                glm::vec3 pos = {
                        //We use 0, 2, 1 due to the coordinate system vulkan uses compared to OpenGL
                        vertexAttributes.vertices[3 * index.vertex_index + 0], //X
                        vertexAttributes.vertices[3 * index.vertex_index + 2], //Z
                        vertexAttributes.vertices[3 * index.vertex_index + 1] //Y
                };

                /*glm::vec2 uvCoord = {
                        vertexAttributes.texcoords[3 * index.texcoord_index + 0],
                        vertexAttributes.texcoords[3 * index.texcoord_index + 1],
                };*/

                glm::vec3 normal = {
                        vertexAttributes.normals[3 * index.normal_index + 0],
                        vertexAttributes.normals[3 * index.normal_index + 2],
                        vertexAttributes.normals[3 * index.normal_index + 1]
                };

                class Vertex vertex = Vertex(pos, normal, color);
                if (newVertices.count(vertex) == 0) {
                    newVertices[vertex] = newVertices.size();
                    resultMesh.description.vertices.push_back(vertex);
                }
                resultMesh.description.indices.push_back(newVertices[vertex]);
            }
        }

        return resultMesh;
    }
}
