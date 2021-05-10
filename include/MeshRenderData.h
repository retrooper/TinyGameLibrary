#pragma once
#include <glm/glm.hpp>
namespace tgl {
    struct MeshRenderData {
        glm::mat4 model;
        glm::vec3 lightPos;
        glm::vec3 cameraPos;
        float time;
    };
}