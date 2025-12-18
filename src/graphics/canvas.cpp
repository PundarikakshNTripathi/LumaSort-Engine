#include "canvas.h"
#include <iostream>
#include <vector>
#include <cmath>

Canvas::Canvas(int width, int height)
    : m_width(width), m_height(height), m_fbo(0), m_lineVAO(0), m_lineVBO(0), m_shaderProgram(0)
{
    initGL();
    initShader();
    clear(); // Start black
}

Canvas::~Canvas() {
    if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
    if (m_lineVAO) glDeleteVertexArrays(1, &m_lineVAO);
    if (m_lineVBO) glDeleteBuffers(1, &m_lineVBO);
    if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
}

void Canvas::initGL() {
    // 1. Create Texture
    m_texture.allocate(m_width, m_height, GL_RGB8, GL_RGB);

    // 2. Create FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.getID(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Canvas::initGL: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. Setup Line VAO/VBO
    glGenVertexArrays(1, &m_lineVAO);
    glGenBuffers(1, &m_lineVBO);

    glBindVertexArray(m_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Canvas::initShader() {
    const char* vertexSrc = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        uniform vec2 uResolution;
        void main() {
            vec2 zeroOne = aPos / uResolution;
            vec2 zeroTwo = zeroOne * 2.0;
            vec2 clipSpace = zeroTwo - 1.0;
            gl_Position = vec4(clipSpace.x, -clipSpace.y, 0.0, 1.0);
        }
    )";

    const char* fragmentSrc = R"(
        #version 330 core
        uniform vec3 uColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(uColor, 1.0);
        }
    )";

    auto compile = [](GLenum type, const char* src) {
        unsigned int id = glCreateShader(type);
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);
        int success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(id, 512, nullptr, infoLog);
            std::cerr << "Shader Compile Error: " << infoLog << std::endl;
        }
        return id;
    };

    unsigned int vs = compile(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fs = compile(GL_FRAGMENT_SHADER, fragmentSrc);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vs);
    glAttachShader(m_shaderProgram, fs);
    glLinkProgram(m_shaderProgram);
    
    int success;
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Program Link Error: " << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Canvas::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::fill(glm::vec3 color) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::drawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color, float brushSize) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    
    glUseProgram(m_shaderProgram);
    
    int resLoc = glGetUniformLocation(m_shaderProgram, "uResolution");
    glUniform2f(resLoc, (float)m_width, (float)m_height);
    
    int colorLoc = glGetUniformLocation(m_shaderProgram, "uColor");
    glUniform3f(colorLoc, color.r, color.g, color.b);

    // Draw smooth line by interpolating points along the path
    float dist = glm::length(end - start);
    int steps = std::max(1, (int)(dist / (brushSize * 0.3f)));
    
    for (int i = 0; i <= steps; ++i) {
        float t = (steps > 0) ? (float)i / (float)steps : 0.0f;
        glm::vec2 pos = start + t * (end - start);
        
        // Draw a point by drawing multiple lines in a circle pattern
        for (int angle = 0; angle < 8; ++angle) {
            float rad = (float)angle * 3.14159f / 4.0f;
            float halfSize = brushSize * 0.5f;
            glm::vec2 offset1(cos(rad) * halfSize, sin(rad) * halfSize);
            glm::vec2 offset2(cos(rad + 3.14159f) * halfSize, sin(rad + 3.14159f) * halfSize);
            
            glm::vec2 verts[2] = { pos + offset1, pos + offset2 };
            
            glBindVertexArray(m_lineVAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
            
            glLineWidth(2.0f);
            glDrawArrays(GL_LINES, 0, 2);
        }
    }
    
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Reads the canvas texture back to CPU as an OpenCV Mat.
 * 
 * Captures the current FBO contents and returns them as a BGR OpenCV matrix.
 * Handles pixel alignment to prevent stride artifacts on certain GPUs.
 * 
 * @return cv::Mat BGR image of the canvas contents.
 * 
 * @note The image is flipped vertically to match OpenCV's top-left origin
 *       convention (OpenGL uses bottom-left origin).
 * @note GL_PACK_ALIGNMENT is set to 1 to match OpenCV's tight packing,
 *       addressing GitHub Issue #8.
 */
cv::Mat Canvas::getAsMat() const {
    cv::Mat result(m_height, m_width, CV_8UC3);

    // Set pack alignment to 1 byte to match OpenCV's tightly-packed layout.
    // Prevents stride artifacts when reading pixels back to CPU.
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glReadPixels(0, 0, m_width, m_height, GL_BGR, GL_UNSIGNED_BYTE, result.data);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Restore default alignment to avoid affecting other OpenGL operations
    glPixelStorei(GL_PACK_ALIGNMENT, 4);

    // Flip vertically because OpenGL origin is bottom-left
    cv::flip(result, result, 0);

    return result;
}
