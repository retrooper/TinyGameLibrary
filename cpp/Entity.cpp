#include "Entity.h"

namespace tgl {

    Entity::Entity(const Mesh &mesh) {
        this->mesh = mesh;
        this->scale = {1, 1, 1};
    }

    Entity::Entity(const Mesh &mesh, glm::vec3 position, float pitch, float yaw, glm::vec3 scale) {
        this->mesh = mesh;
        this->position = position;
        this->pitch = pitch;
        this->yaw = yaw;
        this->scale = scale;
    }
}