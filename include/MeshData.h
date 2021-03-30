#pragma once
#include <glm/glm.hpp>
namespace tgl {
    struct MeshData {
        glm::mat4 model;
        glm::vec3 lightPos;
    };
}