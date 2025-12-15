#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <opencv2/opencv.hpp>
#include "texture.h"

/**
 * @class Canvas
 * @brief Represents a drawing pad backed by a Framebuffer Object (FBO).
 *
 * Allows drawing colored lines onto a background using mouse interaction.
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
     * @param color RGB color (0-1 range).
     * @param brushSize Thickness of the line.
     */
    void drawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color = glm::vec3(1.0f), float brushSize = 2.0f);

    /**
     * @brief Fills the entire canvas with a color.
     * 
     * @param color RGB color (0-1 range).
     */
    void fill(glm::vec3 color);

    /**
     * @brief Clears the canvas to white.
     */
    void clear();

    /**
     * @brief Gets the canvas content as OpenCV Mat.
     * 
     * @return cv::Mat The canvas pixels in BGR format.
     */
    cv::Mat getAsMat() const;

    /**
     * @brief Gets the texture containing the drawing.
     * 
     * @return const Texture2D& The backing texture.
     */
    const Texture2D& getTexture() const { return m_texture; }

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

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
