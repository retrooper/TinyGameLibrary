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
        glm::vec2 mousePos = glm::vec2(posX / window.width, posY / window.height) * 2.0f - 1.0f;
        float yaw = mousePos.x * camera.sensitivity * M_PI;
        float pitch = -mousePos.y * camera.sensitivity * M_PI_2;
        if (pitch > M_PI_2) {
            pitch = M_PI_2;
        } else if (pitch < -M_PI_2) {
            pitch = -M_PI_2;
        }
        camera.yaw = yaw;
        camera.pitch = pitch;
    }
}

void handleMovement(const int &key, Renderer &renderer, Camera &camera, const double &deltaTime) {
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
            camera.position += camera.forwardLinear * deltaTimeVec;
            break;
        case GLFW_KEY_S:
            deltaTimeVec *= glm::vec3(backSpeed, backSpeed, backSpeed);
            camera.position += -camera.forwardLinear * deltaTimeVec;
            break;
        case GLFW_KEY_A:
            deltaTimeVec *= glm::vec3(leftSpeed, leftSpeed, leftSpeed);
            camera.position += -camera.right * deltaTimeVec;
            break;
        case GLFW_KEY_D:
            deltaTimeVec *= glm::vec3(rightSpeed, rightSpeed, rightSpeed);
            camera.position += camera.right * deltaTimeVec;
            break;
        case GLFW_KEY_SPACE:
            camera.position.y -= upSpeed * deltaTime;
            break;
        case GLFW_KEY_LEFT_ALT:
            camera.position.y += downSpeed * deltaTime;
            break;
        default:
            break;
    }
}

void updateCamera(Camera &camera, Window &window, Renderer &renderer, double deltaTime) {
    int cursorStatus = glfwGetInputMode(window.glfwWindow, GLFW_CURSOR);
    if (cursorStatus == GLFW_CURSOR_DISABLED) {
        if (glfwGetKey(window.glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_W, renderer, camera, deltaTime);
        } else if (glfwGetKey(window.glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_S, renderer, camera, deltaTime);
        }

        if (glfwGetKey(window.glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_A, renderer, camera, deltaTime);
        } else if (glfwGetKey(window.glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_D, renderer, camera, deltaTime);
        }

        if (glfwGetKey(window.glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_SPACE, renderer, camera, deltaTime);
        } else if (glfwGetKey(window.glfwWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            handleMovement(GLFW_KEY_LEFT_ALT, renderer, camera, deltaTime);
        }
    }
}

int main() {
    //Initialize TGL
    TGL::init();
    //Create window
    window = Window("Test Window", 1280, 720, false, {0, 0, 1, 1});
    //Display the window and create a surface to render on
    window.create();
    glfwSetKeyCallback(window.glfwWindow, keyCallback);
    glfwSetCursorPosCallback(window.glfwWindow, mouseCallback);
    //Create renderer
    Renderer renderer(&window, 3);
    renderer.init();

    std::vector<Entity> entities;
    for (uint32_t i = 0; i < 1; i++) {
        Entity entity;
        entity.mesh = MeshLoader::loadObj("../resources/models/IronMan.obj", {1, 0, 0, 1});
        entity.mesh.description.vertices.clear();
        entity.mesh.description.indices.clear();

        //top left - 0
        entity.mesh.description.vertices.push_back(Vertex({-1.0f, -1.0f, 0.0f}, {0, 0, 0}, {1, 0, 0, 1}));
        //top right - 1
        entity.mesh.description.vertices.push_back(Vertex({1.0f, -1.0f, 0.0f}, {0, 0, 0}, {1, 0, 0, 1}));
        //bottom left - 2
        entity.mesh.description.vertices.push_back(Vertex({-1.0f, 1.0f, 0.0f}, {0, 0, 0}, {1, 0, 0, 1}));
        //bottom right - 3
        entity.mesh.description.vertices.push_back(Vertex({1.0f, 1.0f, 0.0f}, {0, 0, 0}, {1, 0, 0, 1}));;
        /*
        entity.mesh.description.indices.push_back(0);
        entity.mesh.description.indices.push_back(1);
        entity.mesh.description.indices.push_back(2);
        entity.mesh.description.indices.push_back(2);
        entity.mesh.description.indices.push_back(1);
        entity.mesh.description.indices.push_back(3);
         */
        entity.mesh.description.indices = {2, 0, 3, 0, 1, 3};
        renderer.uploadMesh(entity.mesh);
        entities.push_back(entity);
    }
    GPU gpu = renderer.gpu;
    std::cout << "GPU NAME: " << gpu.name << std::endl;
    std::cout << "GPU TYPE: " << gpu.type << std::endl;
    std::cout << "VULKAN VERSION API VERSION: " << gpu.vulkanAPIVersionSupported[0] << "."
              << gpu.vulkanAPIVersionSupported[1] << "." << gpu.vulkanAPIVersionSupported[2] << std::endl;
    std::cout << "GPU DRIVER VERSION: " << gpu.driverVersion << std::endl;

    camera.farClipPlane = 100;
    camera.nearClipPlane = 0.1f;
    camera.fov = 80;
    camera.position = {0, 0, 0};

    camera.sensitivity = 1;
    double deltaTime, lastFrameTime;
    Light light{};
    light.position = {0, -6, 0};
    while (!window.hasRequestedClose()) {
        //Update the window events. We need this to detect if they requested to close the window for example.
        renderer.registerEntities(entities);
        window.updateEvents();
        renderer.render(camera, light);

        renderer.clearEntities();
        updateCamera(camera, window, renderer, deltaTime);
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
