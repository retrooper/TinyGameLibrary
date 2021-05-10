#include "Ray.h"
namespace tgl {
    Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) {
        this->origin = origin;
        this->direction = direction;
    }

    glm::vec3 Ray::at(const float& distance) const {
        return origin + (distance * direction);
    }
}