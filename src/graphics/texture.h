#pragma once

#include <glad/glad.h>
#include <opencv2/opencv.hpp>

/**
 * @class Texture2D
 * @brief Wrapper for an OpenGL Texture 2D object.
 *
 * Handles creation, destruction, and uploading of data from OpenCV matrices.
 */
class Texture2D {
public:
    /**
     * @brief Constructs a new Texture2D object.
     */
    Texture2D();

    /**
     * @brief Destroys the Texture2D object and deletes the GL texture.
     */
    ~Texture2D();

    /**
     * @brief Binds the texture to a specific texture unit.
     * 
     * @param unit Texture unit to bind to (default 0).
     */
    void bind(unsigned int unit = 0) const;

    /**
     * @brief Uploads image data from an OpenCV matrix to the GPU.
     * 
     * Handles BGR to RGB swizzling automatically.
     * 
     * @param mat The source OpenCV matrix.
     */
    void uploadFromOpenCV(const cv::Mat& mat);

    /**
     * @brief Allocates texture memory without data.
     * 
     * @param width Width in pixels.
     * @param height Height in pixels.
     * @param internalFormat OpenGL internal format (default GL_RGB8).
     * @param format OpenGL data format (default GL_RGB).
     */
    void allocate(int width, int height, GLenum internalFormat = GL_RGB8, GLenum format = GL_RGB);

    /**
     * @brief Gets the OpenGL texture ID.
     * 
     * @return unsigned int The texture ID.
     */
    unsigned int getID() const { return m_rendererID; }

    /**
     * @brief Gets the width of the texture.
     * 
     * @return int Width in pixels.
     */
    int getWidth() const { return m_width; }

    /**
     * @brief Gets the height of the texture.
     * 
     * @return int Height in pixels.
     */
    int getHeight() const { return m_height; }

private:
    unsigned int m_rendererID; ///< OpenGL Texture ID
    int m_width;               ///< Texture width
    int m_height;              ///< Texture height
    GLenum m_internalFormat;   ///< Internal OpenGL format
    GLenum m_dataFormat;       ///< Data format of the pixels
};
