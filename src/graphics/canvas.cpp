#include "canvas.h"
#include <iostream>
#include <vector>

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
    // Buffer for 2 points (Start, End)
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
        void main() {
            // Map [0, width] -> [-1, 1]
            // We expect input in Normalized Device Coordinates (NDC) or we can use a uniform for ortho projection.
            // For simplicity, let's assume the input to drawLine is already mapped or we simple map it here.
            // Let's pass raw NDC coordinates for now, or use a simple orthographic logic.
            // But wait, drawLine receives expected pixel coordinates or window coordinates?
            // Usually mouse gives window coords. Let's map window coords to NDC here.
            
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";
    // Wait, the vertex shader above expects NDC. 
    // It's better to pass a uniform for resolution to map pixel coords to NDC.
    // Let's rewrite:

    const char* vertexSrcFixed = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        uniform vec2 uResolution;
        void main() {
            // Convert pixel (0..width, 0..height) to NDC (-1..1, -1..1)
            // Y is usually inverted in window coords vs OpenGL.
            // Let's assume (0,0) is top-left from mouse, but OpenGL needs (0,0) center, Y up.
            
            vec2 zeroOne = aPos / uResolution;
            vec2 zeroTwo = zeroOne * 2.0;
            vec2 clipSpace = zeroTwo - 1.0;
            
            // Flip Y because mouse is top-down, GL is bottom-up
            gl_Position = vec4(clipSpace.x, -clipSpace.y, 0.0, 1.0);
        }
    )";

    const char* fragmentSrc = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White
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

    unsigned int vs = compile(GL_VERTEX_SHADER, vertexSrcFixed);
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::drawLine(glm::vec2 start, glm::vec2 end, float brushSize) {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    
    glUseProgram(m_shaderProgram);
    
    int loc = glGetUniformLocation(m_shaderProgram, "uResolution");
    glUniform2f(loc, (float)m_width, (float)m_height);

    glLineWidth(brushSize); // Note: glLineWidth > 1.0 is deprecated in Core Profile and might not work on all drivers.
                            // But for a simple prototype it's often supported or we get 1px lines.
                            // Truly thick lines require quad rendering. adhering to "Simulating a drawing pad", we'll try this.

    glm::vec2 verts[2] = { start, end };
    
    glBindVertexArray(m_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    
    glDrawArrays(GL_LINES, 0, 2);
    
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
