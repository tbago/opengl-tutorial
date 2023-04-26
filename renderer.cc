#include "renderer.h"
#include <iostream>
#include <glfw/glfw3.h>

void GLClearError() {
    // clang-format off
    while (glGetError() != GL_NO_ERROR);
    // clang-format on
}

bool GLCheckError(const char *function, const char *file, int line) {
    GLenum error = GL_NO_ERROR;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "[OpenGL error] (" << error << ") " 
                  << function << " "
                  << file << ":" << line << std::endl;
        return true;
    }
    return false;
}
