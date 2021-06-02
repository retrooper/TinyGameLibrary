#pragma once
#include "Mesh.h"
namespace tgl {
    class Entity {
    public:
        glm::vec3 position{};
        float pitch, yaw, roll;
        glm::vec3 scale;
        Mesh mesh;
        bool registered;
        Entity() = default;
        explicit Entity(const Mesh& mesh);
        Entity(const Mesh& mesh, glm::vec3 position, float pitch, float yaw, glm::vec3 scale);
    };
}