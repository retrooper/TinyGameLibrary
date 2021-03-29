#pragma once
#include "Window.h"

namespace tgl {
    Window::Window(const char *title, int width, int height, bool resizable) {
        this->title = title;
        this->width = width;
        this->height = height;
        this->resizable = resizable;
        this->glfwWindow = nullptr;
    }

    void Window::create() {
        if (!hasCreated()) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
            glfwWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
        }
        else {
            //TODO warn them they are creating twice
        }
    }

    bool Window::hasCreated() {
        return glfwWindow != nullptr;
    }

    void Window::updateEvents() {
        glfwPollEvents();
    }

    bool Window::hasRequestedClose() {
        return glfwWindowShouldClose(glfwWindow);
    }

    void Window::destroy() {
        glfwDestroyWindow(glfwWindow);
    }
}