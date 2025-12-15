#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include "texture.h"

/**
 * @class Canvas
 * @brief Represents a drawing pad backed by a Framebuffer Object (FBO).
 *
 * Allows drawing white lines onto a black background using mouse interaction.
 * The result is stored in an OpenGL texture.
 */
class Canvas {
public:
    /**
     * @brief Constructs a new Canvas.
     * 
     * @param width Width of the canvas.
     * @param height Height of the canvas.
     */
    Canvas(int width, int height);

    /**
     * @brief Destroys the Canvas and clean up GL resources.
     */
    ~Canvas();

    /**
     * @brief Draws a line segment on the canvas.
     * 
     * @param start Start position (x, y).
     * @param end End position (x, y).
     * @param brushSize Thickness of the line.
     */
    void drawLine(glm::vec2 start, glm::vec2 end, float brushSize = 2.0f);

    /**
     * @brief Clears the canvas to black.
     */
    void clear();

    /**
     * @brief Gets the texture containing the drawing.
     * 
     * @return const Texture2D& The backing texture.
     */
    const Texture2D& getTexture() const { return m_texture; }

private:
    void initGL();
    void initShader();

    int m_width;
    int m_height;
    unsigned int m_fbo;
    Texture2D m_texture;

    unsigned int m_lineVAO;
    unsigned int m_lineVBO;
    unsigned int m_shaderProgram;
};
