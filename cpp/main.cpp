#include <iostream>
#include "Renderer.h"
#include "Window.h"
#include "TGL.h"

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
    Mesh triangleMesh;
    triangleMesh.vertices.resize(3);
    triangleMesh.vertices[0].position = { 1.f, 1.f, 0.0f };
    triangleMesh.vertices[1].position = {-1.f, 1.f, 0.0f };
    triangleMesh.vertices[2].position = { 0.f,-1.f, 0.0f };
    triangleMesh.vertices[0].color = { 0.f, 1.f, 0.0f, 1.0f };
    triangleMesh.vertices[1].color = { 0.f, 1.f, 0.0f, 1.0f };
    triangleMesh.vertices[2].color = { 0.f, 1.f, 0.0f, 1.0f };
    renderer.uploadMesh(triangleMesh);
    GPU gpu = renderer.gpu;
    std::cout << "GPU NAME: " << gpu.name << std::endl;
    std::cout << "VULKAN VERSION API VERSION: " << gpu.vulkanAPIVersionSupported[0] << "."
              << gpu.vulkanAPIVersionSupported[1] << "." << gpu.vulkanAPIVersionSupported[2] << std::endl;
    std::cout << "GPU DRIVER VERSION: " << gpu.driverVersion << std::endl;
    std::cout << "GPU TYPE: " << gpu.type << std::endl;
    //Keep checking if the user hasn't requested to close the window.
    double startTime = glfwGetTime();
    while (!window.hasRequestedClose()) {
        //Update the window events. We need this to detect if they requested to close the window for example.
        window.updateEvents();
       renderer.render(triangleMesh);
       renderer.render(triangleMesh);
    }
    //Destroy the renderer
    renderer.destroy();
    //Destroy the window
    window.destroy();
    //Terminate TGL
    TGL::terminate();
    return 0;
}
