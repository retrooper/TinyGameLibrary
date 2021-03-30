#include "Entity.h"

namespace tgl {

    Entity::Entity(const Mesh &mesh) {
        this->mesh = mesh;
        this->scale = {1, 1, 1};
    }

    Entity::Entity(const Mesh &mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
        this->mesh = mesh;
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
    }
}