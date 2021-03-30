#pragma once
#include "Mesh.h"
namespace tgl {
    class Entity {
    public:
        glm::vec3 position{};
        glm::vec3 rotation{};
        glm::vec3 scale;
        Mesh mesh;
        Entity() = default;
        explicit Entity(const Mesh& mesh);
        Entity(const Mesh& mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
    };
}