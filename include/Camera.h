#pragma once
#include <glm/glm.hpp>
#include "CameraData.h"
namespace tgl {
    struct MousePos {
        double posX, posY;
        double lastPosX, lastPosY;
        double deltaPosX, deltaPosY;
        bool firstRotation = true;
    };
    class Camera {
    public:
        glm::vec3 cameraFront;
        glm::vec3 position;
        float yaw, pitch;
        MousePos mousePos;
        double sensitivity = 100;
        float fov = 80;
        float nearClipPlane = 0.1f;
        float farClipPlane = 100.0f;
        CameraData data;
    };
}