#include "renderer.h"
#include <glad/glad.h>
#include <iostream>

namespace Graphics {

    Renderer::Renderer() {
        // Load OpenGL function pointers via GLAD.
        // This is critical because modern OpenGL functions are determined at runtime.
        // Note: gladLoadGLLoader is usually called in App::init, but we can double check here
        // or load specific extensions if needed.
        // For now, checks are done in App::init.
    }

    Renderer::~Renderer() {
    }

    void Renderer::clear() {
        // Set a dark gray background to make the pixel sorter pop.
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        
        // Clear the color buffer bit to apply the background color.
        glClear(GL_COLOR_BUFFER_BIT);
    }

}
