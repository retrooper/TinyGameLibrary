#include <iostream>
#include "Renderer.h"
#include "Window.h"
#include "TGL.h"
#include "MeshLoader.h"

using namespace tgl;

Camera camera;
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        int cursorStatus = glfwGetInputMode(window, GLFW_CURSOR);
        if (cursorStatus == GLFW_CURSOR_NORMAL) {
            cursorStatus = GLFW_CURSOR_DISABLED;
        }
        else {
            cursorStatus = GLFW_CURSOR_NORMAL;
        }
        glfwSetInputMode(window, GLFW_CURSOR, cursorStatus);
    }
}

static void mouseCallback(GLFWwindow* window, double posX, double posY) {
    int cursorStatus = glfwGetInputMode(window, GLFW_CURSOR);
    if (cursorStatus == GLFW_CURSOR_DISABLED) {
        if (camera.mousePos.firstRotation) {
            camera.mousePos.lastPosX = posX;
            camera.mousePos.lastPosY = posY;
            camera.mousePos.firstRotation = false;
        }
        float sens = camera.sensitivity / 1000;
        double xOffset = (posX - camera.mousePos.lastPosX) * sens;
        double yOffset = (posY - camera.mousePos.lastPosY) * sens;
        camera.mousePos.lastPosX = posX;
        camera.mousePos.lastPosY = posY;
        camera.yaw = glm::mod(camera.yaw + (float) xOffset, 360.0f);
        camera.pitch += (float) yOffset;

        if (camera.pitch > 89.0f) {
            camera.pitch = 89.0f;
        }
        if (camera.pitch < -89.0f) {
            camera.pitch = -89.0f;
        }
    }
}

double to_radians(double a) {
    return a * (3.14 / 180);
}

void handleMovement(int key, Camera& camera, double deltaTime) {
    double yaw = to_radians(camera.yaw);
    double frontSpeed = 1;
    double backSpeed = 1;
    double leftSpeed = 1;
    double rightSpeed = 1;
    double upSpeed = 1;
    double downSpeed = 1;
    glm::vec3 forwardVec = {sin(-yaw) * frontSpeed * deltaTime, -cos(yaw) * frontSpeed * deltaTime, 0};
    switch (key) {
        case GLFW_KEY_W:
            camera.position += forwardVec;
            break;
        case GLFW_KEY_S:
            camera.position += forwardVec * glm::vec3(-1, -1, -1);
            break;
        case GLFW_KEY_A:
            camera.position.x -= forwardVec.y;
            camera.position.y += forwardVec.x;
            break;
        case GLFW_KEY_D:
            camera.position.x += forwardVec.y;
            camera.position.y -= forwardVec.x;
            break;
        case GLFW_KEY_SPACE:
            camera.position.z += upSpeed * deltaTime;
            break;
        case GLFW_KEY_LEFT_ALT:
            camera.position.z -= downSpeed * deltaTime;
            break;
    }
}

void updateCamera(Camera& camera, Window& window, double deltaTime) {
    int cursorStatus = glfwGetInputMode(window.glfwWindow, GLFW_CURSOR);
    if (cursorStatus == GLFW_CURSOR_DISABLED) {
        if (glfwGetKey(window.glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_W, camera, deltaTime);
        }
        else  if (glfwGetKey(window.glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_S, camera, deltaTime);
        }

        if (glfwGetKey(window.glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_A, camera, deltaTime);
        }
        else  if (glfwGetKey(window.glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_D, camera, deltaTime);
        }

        if (glfwGetKey(window.glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_SPACE, camera, deltaTime);
        }
        else  if (glfwGetKey(window.glfwWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_LEFT_ALT, camera, deltaTime);
        }
    }
}

int main() {
    //Initialize TGL
    TGL::init();
    //Create window
    Window window("Test Window", 1280, 720, false);
    //Display the window and create a surface to render on
    window.create();
    glfwSetKeyCallback(window.glfwWindow, keyCallback);
    glfwSetCursorPosCallback(window.glfwWindow, mouseCallback);
    //Create renderer
    Renderer renderer(&window);
    renderer.init();
    Light light;
    light.position = {0, 0, 2};
    Entity dragon;
    dragon.mesh = MeshLoader::loadObj("../resources/models/cube.obj", {0, 1, 0, 1});
    dragon.position.z -= 2;
    dragon.scale = {0.5, 0.5, 0.5};
    dragon.rotation.x = 10;
    Entity dragon2;
    dragon2.mesh = MeshLoader::loadObj("../resources/models/dragon.obj");
    dragon2.scale = {0.1, 0.1, 0.1};
    dragon2.rotation.x = 30;
    renderer.uploadMesh(dragon.mesh);
    renderer.uploadMesh(dragon2.mesh);
    GPU gpu = renderer.gpu;
    std::cout << "GPU NAME: " << gpu.name << std::endl;
    std::cout << "VULKAN VERSION API VERSION: " << gpu.vulkanAPIVersionSupported[0] << "."
              << gpu.vulkanAPIVersionSupported[1] << "." << gpu.vulkanAPIVersionSupported[2] << std::endl;
    std::cout << "GPU DRIVER VERSION: " << gpu.driverVersion << std::endl;
    std::cout << "GPU TYPE: " << gpu.type << std::endl;
    //Keep checking if the user hasn't requested to close the window.
    double startTime = glfwGetTime();
    camera.farClipPlane = 100;
    camera.fov = 80;
    camera.nearClipPlane = 0.1f;
    camera.sensitivity = 100;
    double deltaTime, lastFrameTime;
    while (!window.hasRequestedClose()) {
        //Update the window events. We need this to detect if they requested to close the window for example.
        window.updateEvents();
        renderer.registerEntity(dragon);
        renderer.registerEntity(dragon2);
        renderer.render(camera, light);
        updateCamera(camera, window, deltaTime);
        double now = glfwGetTime() * 1000;
        deltaTime = (now - lastFrameTime) / 1000.0;
        lastFrameTime = now;
    }
    //Destroy the renderer
    renderer.destroy();
    //Destroy the window
    window.destroy();
    //Terminate TGL
    TGL::terminate();
    return 0;
}
