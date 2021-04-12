#include <cmath>
#include <iostream>
#include "Renderer.h"
#include "Window.h"
#include "TGL.h"
#include "MeshLoader.h"
#include <deque>

using namespace tgl;

Camera camera;
Window window;

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        int cursorStatus = glfwGetInputMode(window, GLFW_CURSOR);
        if (cursorStatus == GLFW_CURSOR_NORMAL) {
            cursorStatus = GLFW_CURSOR_DISABLED;
        } else {
            cursorStatus = GLFW_CURSOR_NORMAL;
        }
        glfwSetInputMode(window, GLFW_CURSOR, cursorStatus);
    }
}

static void mouseCallback(GLFWwindow *glfwWindow, double posX, double posY) {
    int cursorStatus = glfwGetInputMode(glfwWindow, GLFW_CURSOR);
    if (cursorStatus == GLFW_CURSOR_DISABLED) {
        double sens = camera.sensitivity / 10;
        glm::vec2 mousePos = glm::vec2(posX / window.width, posY / window.height) * 2.0f - 1.0f;
        camera.yaw = -mousePos.x * sens * M_PIf32;
        camera.pitch = mousePos.y * sens * M_2_PIf32;
    }
}

glm::vec3 rot(glm::vec3 a, float angle){
    glm::vec3 g = glm::vec3(std::cos(angle), std::sin(angle), 0);
    return glm::vec3(a.x*g.x - a.y*g.y, a.x*g.y + a.y*g.x, 0);
}

void handleMovement(int key, Camera &camera, double deltaTime) {
    double frontSpeed = 2;
    double backSpeed = 2;
    double leftSpeed = 2;
    double rightSpeed = 2;
    double upSpeed = 2;
    double downSpeed = 2;
    glm::vec3 deltaTimeVec = {deltaTime, deltaTime, deltaTime};
    switch (key) {
        case GLFW_KEY_W:
            deltaTimeVec *= glm::vec3(frontSpeed, frontSpeed, frontSpeed);
            camera.position += rot(camera.forward, camera.yaw) * deltaTimeVec;
            break;
        case GLFW_KEY_S:
            deltaTimeVec *= glm::vec3(backSpeed, backSpeed, backSpeed);
            camera.position += rot(camera.back, camera.yaw) * deltaTimeVec;
            break;
        case GLFW_KEY_A:
            deltaTimeVec *= glm::vec3(leftSpeed, leftSpeed, leftSpeed);
            camera.position += rot(camera.left, camera.yaw) * deltaTimeVec;
            break;
        case GLFW_KEY_D:
            deltaTimeVec *= glm::vec3(rightSpeed, rightSpeed, rightSpeed);
            camera.position += rot(camera.right, camera.yaw) * deltaTimeVec;
            break;
        case GLFW_KEY_SPACE:
            camera.position.z += upSpeed * deltaTime;
            break;
        case GLFW_KEY_LEFT_ALT:
            camera.position.z -= downSpeed * deltaTime;
            break;
        default:
            break;
    }
}

void updateCamera(Camera &camera, Window &window, double deltaTime) {
    int cursorStatus = glfwGetInputMode(window.glfwWindow, GLFW_CURSOR);
    if (cursorStatus == GLFW_CURSOR_DISABLED) {
        if (glfwGetKey(window.glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_W, camera, deltaTime);
        } else if (glfwGetKey(window.glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_S, camera, deltaTime);
        }

        if (glfwGetKey(window.glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_A, camera, deltaTime);
        } else if (glfwGetKey(window.glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_D, camera, deltaTime);
        }

        if (glfwGetKey(window.glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_SPACE, camera, deltaTime);
        } else if (glfwGetKey(window.glfwWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_LEFT_ALT, camera, deltaTime);
        }
    }
}

int main() {
    //Initialize TGL
    TGL::init();
    //Create window
    window = Window("Test Window", 800, 600, false);
    //Display the window and create a surface to render on
    window.create();
    glfwSetKeyCallback(window.glfwWindow, keyCallback);
    glfwSetCursorPosCallback(window.glfwWindow, mouseCallback);
    //Create renderer
    Renderer renderer(&window, 3);
    renderer.init();
    std::deque<Entity> entities;
    Entity dragon;
    dragon.scale = {0.05, 0.05, 0.05};
    dragon.mesh = MeshLoader::loadObj("../resources/models/IronMan.obj");
    entities.push_back(dragon);
    GPU gpu = renderer.gpu;
    std::cout << "GPU NAME: " << gpu.name << std::endl;
    std::cout << "VULKAN VERSION API VERSION: " << gpu.vulkanAPIVersionSupported[0] << "."
              << gpu.vulkanAPIVersionSupported[1] << "." << gpu.vulkanAPIVersionSupported[2] << std::endl;
    std::cout << "GPU DRIVER VERSION: " << gpu.driverVersion << std::endl;
    std::cout << "GPU TYPE: " << gpu.type << std::endl;
    //Keep checking if the user hasn't requested to close the window.
    camera.farClipPlane = 100;
    camera.fov = 80;
    camera.nearClipPlane = 0.1f;
    camera.sensitivity = 1;
    double deltaTime, lastFrameTime;
    Light light{};
    light.position = {0, 3, 3};
    for (auto &e : entities) {
        renderer.registerEntity(e);
    }
    while (!window.hasRequestedClose()) {
        //Update the window events. We need this to detect if they requested to close the window for example.
        window.updateEvents();
        renderer.render(camera, light);
        updateCamera(camera, window, deltaTime);
        double now = glfwGetTime() * 1000;
        deltaTime = (now - lastFrameTime) / 1000.0;
        lastFrameTime = now;
        //std::cout << "X: " << camera.position.x << ", Y: " << camera.position.y << ", Z: " << camera.position.z << ", Pitch: " << camera.pitch << ", Yaw: " << camera.yaw << std::endl;
    }
    //Destroy the renderer
    renderer.destroy();
    //Destroy the window
    window.destroy();
    //Terminate TGL
    TGL::terminate();
    return 0;
}
