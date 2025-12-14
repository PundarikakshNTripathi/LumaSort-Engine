#include "renderer.h"
#include <glad/glad.h>
#include <iostream>

namespace Graphics {

    Renderer::Renderer() {
        if (!gladLoadGL()) {
            std::cerr << "Failed to initialize GLAD inside Renderer" << std::endl;
        }
    }

    Renderer::~Renderer() {
    }

    void Renderer::clear() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

}
