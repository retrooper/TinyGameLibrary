#pragma once
#include <glm/glm.hpp>
#include "CameraData.h"
namespace tgl {
    class Camera {
    public:
        glm::vec3 position, forward, forwardLinear, right;
        float pitch = 0, yaw = 0, roll = 0;
        float sensitivity = 100.0f;
        float fov = 80;
        float nearClipPlane = 0.1f;
        float farClipPlane = 100.0f;
        CameraData data;
    };
}