#pragma once
#include "GLFWIncluder.h"
namespace tgl {
    class Window {
        friend class Renderer;
    public:
        GLFWwindow* glfwWindow;

        const char* title;
        int width, height;
        bool resizable;
        Window(const char* title, int width, int height, bool resizable = false);

        void create();
        bool hasCreated();
        void updateEvents();
        bool hasRequestedClose();
        void destroy();
    };
}