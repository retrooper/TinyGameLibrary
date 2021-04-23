#pragma once
#include <glm/glm.hpp>
#include "CameraData.h"
namespace tgl {
    class Camera {
    public:
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;
        double yaw, pitch;
        double sensitivity = 100;
        float fov = 80;
        float nearClipPlane = 0.1f;
        float farClipPlane = 100.0f;
        CameraData data;
    };
}