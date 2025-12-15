#include "renderer.h"
#include <glad/glad.h>
#include <iostream>

namespace Graphics {

    // Simple Shader Source
    const char* particleVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec4 aColor;
        out vec4 vColor;
        void main() {
            // Map 0..1 (Image Coords) to -1..1 (NDC)
            // Flip Y so 0 is top
            gl_Position = vec4(aPos.x * 2.0 - 1.0, (1.0 - aPos.y) * 2.0 - 1.0, 0.0, 1.0);
            gl_PointSize = 2.0;
            vColor = aColor;
        }
    )";

    const char* particleFragmentShaderSource = R"(
        #version 330 core
        in vec4 vColor;
        out vec4 FragColor;
        void main() {
            FragColor = vColor;
        }
    )";

    Renderer::Renderer() {
        // Compile Shaders
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &particleVertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // Check errors (omitted for brevity, but crucial in prod)

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &particleFragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        m_ParticleShader = glCreateProgram();
        glAttachShader(m_ParticleShader, vertexShader);
        glAttachShader(m_ParticleShader, fragmentShader);
        glLinkProgram(m_ParticleShader);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Setup Buffers
        glGenVertexArrays(1, &m_ParticleVAO);
        glGenBuffers(1, &m_ParticleVBO);

        glBindVertexArray(m_ParticleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
        
        // Stride is sizeof(Particle).
        // Particle struct: pos(vec2), vel, acc, target, color(vec4)
        // We only send the whole struct to GPU? Or just pos/color?
        // Ideally just build a vector of pos/color, but that's extra copy.
        // We can bind the whole struct and use stride/offset.
        // sizeof(Particle) = 2*4 + 2*4 + 2*4 + 2*4 + 4*4 = 48 bytes (floats).
        
        GLsizei stride = sizeof(Particle);
        
        // Position (0)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);

        // Color (1) - Offset is 4 * sizeof(vec2) = 4 * 8 = 32 bytes
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(glm::vec2)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0);
    }

    Renderer::~Renderer() {
        glDeleteVertexArrays(1, &m_ParticleVAO);
        glDeleteBuffers(1, &m_ParticleVBO);
        glDeleteProgram(m_ParticleShader);
    }

    void Renderer::clear() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer::renderParticles(const std::vector<Particle>& particles) {
        if (particles.empty()) return;

        glUseProgram(m_ParticleShader);
        glBindVertexArray(m_ParticleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);

        // Upload data
        // Using GL_STREAM_DRAW because we update every frame
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STREAM_DRAW);

        glEnable(GL_PROGRAM_POINT_SIZE); // Ensure point size works provided shader writes it
        glDrawArrays(GL_POINTS, 0, (GLsizei)particles.size());
        glDisable(GL_PROGRAM_POINT_SIZE);

        glBindVertexArray(0);
        glUseProgram(0);
    }

}
