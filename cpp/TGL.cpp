#include "TGL.h"

void tgl::TGL::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        std::exit(-1);
    }
}

void tgl::TGL::terminate() {
    glfwTerminate();
}
