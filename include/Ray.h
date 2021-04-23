#pragma once
#include "glm/glm.hpp"
namespace tgl {
    struct Ray {
        glm::vec3 origin, direction;
        Ray() = default;
        Ray(const glm::vec3 origin, const glm::vec3 direction);

        glm::vec3 at(float distance) const;
    };
}