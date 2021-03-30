#include <iostream>
#include "Renderer.h"
#include "Window.h"
#include "TGL.h"
#include "MeshLoader.h"

using namespace tgl;

int main() {
    //Initialize TGL
    TGL::init();
    //Create window
    Window window("Test Window", 1280, 720, false);
    //Display the window and create a surface to render on
    window.create();
    //Create renderer
    Renderer renderer(&window);
    renderer.init();
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
    Camera camera{{0,0, -3}};
    while (!window.hasRequestedClose()) {
        //Update the window events. We need this to detect if they requested to close the window for example.
        window.updateEvents();
        renderer.registerEntity(dragon);
        renderer.registerEntity(dragon2);
        renderer.render(camera);
    }
    //Destroy the renderer
    renderer.destroy();
    //Destroy the window
    window.destroy();
    //Terminate TGL
    TGL::terminate();
    return 0;
}
