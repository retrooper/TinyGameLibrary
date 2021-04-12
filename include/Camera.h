#pragma once
#include <glm/glm.hpp>
#include "CameraData.h"
namespace tgl {
    class Camera {
    public:
        glm::vec3 forward = {0, 1, 0};
        glm::vec3 back = -forward;
        glm::vec3 left = {-1, 0, 0};
        glm::vec3 right = -left;
        glm::vec3 position;
        float yaw, pitch;
        double sensitivity = 100;
        float fov = 80;
        float nearClipPlane = 0.1f;
        float farClipPlane = 100.0f;
        CameraData data;
    };
}