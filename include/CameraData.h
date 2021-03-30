#pragma once
#include <glm/glm.hpp>

namespace tgl {
    struct CameraData {
        glm::mat4 view;
        glm::mat4 projection;
    };
}