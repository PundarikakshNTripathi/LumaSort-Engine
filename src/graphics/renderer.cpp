#include "renderer.h"
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace Graphics {

    // Basic file reading helper
    std::string readFile(const char* path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open shader file: " << path << std::endl;
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    Renderer::Renderer() {
        // Compile Shaders from assets
        std::string vertexCode = readFile("assets/shaders/particle.vert");
        std::string fragmentCode = readFile("assets/shaders/particle.frag");
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vShaderCode, NULL);
        glCompileShader(vertexShader);
        // Check errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        m_ParticleShader = glCreateProgram();
        glAttachShader(m_ParticleShader, vertexShader);
        glAttachShader(m_ParticleShader, fragmentShader);
        glLinkProgram(m_ParticleShader);
        
        glGetProgramiv(m_ParticleShader, GL_LINK_STATUS, &success);
        if (!success) {
             glGetProgramInfoLog(m_ParticleShader, 512, NULL, infoLog);
             std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

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

    /**
     * @brief Renders particles as points with dynamic sizing to prevent gaps.
     * 
     * This function calculates the optimal point size based on the ratio between
     * viewport dimensions and simulation grid dimensions. A 1.5x multiplier ensures
     * particles overlap slightly, preventing visible gaps between adjacent particles.
     * 
     * This addresses GitHub Issue #8: On integrated graphics (Intel HD), fixed point
     * sizes cause visible vertical black lines when viewport/simulation ratio exceeds
     * the point size.
     * 
     * @param particles Vector of particles to render.
     * @param viewportWidth Current viewport width in pixels.
     * @param viewportHeight Current viewport height in pixels.
     * @param simWidth Simulation grid width (number of particles horizontally).
     * @param simHeight Simulation grid height (number of particles vertically).
     * 
     * @note Point size formula: max(viewportWidth/simWidth, viewportHeight/simHeight) * 1.5
     * @note Minimum point size is clamped to 1.0 to ensure visibility.
     */
    void Renderer::renderParticles(const std::vector<Particle>& particles, int viewportWidth, int viewportHeight, int simWidth, int simHeight) {
        if (particles.empty()) return;

        glUseProgram(m_ParticleShader);
        
        // Calculate point size to ensure particles fill the viewport without gaps.
        // The 1.5x multiplier creates deliberate overlap to prevent visible stride artifacts.
        float pointSizeX = (float)viewportWidth / (float)simWidth * 1.5f;
        float pointSizeY = (float)viewportHeight / (float)simHeight * 1.5f;
        float pointSize = std::max(pointSizeX, pointSizeY);
        pointSize = std::max(pointSize, 1.0f);  // Clamp to minimum 1.0
        
        // Pass dynamic point size to vertex shader via uniform
        int pointSizeLoc = glGetUniformLocation(m_ParticleShader, "uPointSize");
        glUniform1f(pointSizeLoc, pointSize);
        
        glBindVertexArray(m_ParticleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);

        // Upload particle data - using GL_STREAM_DRAW for per-frame updates
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STREAM_DRAW);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, (GLsizei)particles.size());
        glDisable(GL_PROGRAM_POINT_SIZE);

        glBindVertexArray(0);
        glUseProgram(0);
    }

}
