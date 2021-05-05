#pragma once
#include "GLFWIncluder.h"
#include <glm/vec4.hpp>
namespace tgl {
    class Window {
        friend class Renderer;
    public:
        GLFWwindow* glfwWindow;
        const char* title;
        int width, height;
        glm::vec4 backgroundColor;
        float aspect;
        bool resizable;
        Window() = default;
        Window(const char* title, int width, int height, bool resizable = false, glm::vec4 backgroundColor = {0, 0, 1, 1});

        void create();
        bool hasCreated();
        void updateEvents();
        bool hasRequestedClose();
        void destroy();
    };
}