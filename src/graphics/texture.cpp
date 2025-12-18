#include "texture.h"
#include <iostream>

Texture2D::Texture2D()
    : m_rendererID(0), m_width(0), m_height(0), m_internalFormat(GL_RGB8), m_dataFormat(GL_RGB)
{
    glGenTextures(1, &m_rendererID);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);

    // Set default parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &m_rendererID);
}

void Texture2D::bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);
}

/**
 * @brief Uploads pixel data from an OpenCV Mat to the GPU texture.
 * 
 * This function handles the OpenCV-to-OpenGL data transfer with proper alignment
 * settings to prevent stride artifacts. Key considerations:
 * 
 * 1. **Memory Continuity**: OpenCV Mats may have gaps between rows (e.g., from ROI
 *    operations). Non-continuous Mats are cloned to ensure contiguous memory.
 * 
 * 2. **Pixel Alignment**: OpenGL defaults to 4-byte row alignment, but OpenCV uses
 *    tight packing. We set GL_UNPACK_ALIGNMENT to 1 to prevent stride mismatches
 *    that cause vertical black lines on certain GPUs (especially Intel integrated).
 * 
 * 3. **Color Format**: OpenCV uses BGR ordering; we specify GL_BGR accordingly.
 * 
 * @param mat The source OpenCV matrix (supports 1, 3, or 4 channel images).
 * 
 * @note Addresses GitHub Issue #8: Vertical black stride artifacts on input feed.
 * @warning Empty matrices are rejected with an error message.
 * 
 * @see https://www.khronos.org/opengl/wiki/Common_Mistakes#Texture_upload_and_pixel_reads
 */
void Texture2D::uploadFromOpenCV(const cv::Mat& mat) {
    if (mat.empty()) {
        std::cerr << "Texture2D::uploadFromOpenCV: Empty matrix provided!" << std::endl;
        return;
    }

    // Ensure continuous memory layout - clone if Mat has gaps between rows
    // (can happen with ROI operations or certain OpenCV functions)
    cv::Mat uploadMat = mat.isContinuous() ? mat : mat.clone();

    m_width = uploadMat.cols;
    m_height = uploadMat.rows;

    GLenum format = GL_RGB;
    if (uploadMat.channels() == 4) {
        format = GL_RGBA;
        m_internalFormat = GL_RGBA8;
    } else if (uploadMat.channels() == 3) {
        format = GL_BGR; // OpenCV uses BGR by default
        m_internalFormat = GL_RGB8;
    } else if (uploadMat.channels() == 1) {
        format = GL_RED;
        m_internalFormat = GL_R8;
    }

    m_dataFormat = format;

    glBindTexture(GL_TEXTURE_2D, m_rendererID);

    // Set pixel row alignment to 1 byte to match OpenCV's tightly-packed layout.
    // OpenGL defaults to 4-byte alignment, causing artifacts when row size isn't
    // divisible by 4 (e.g., 641×3 = 1923 bytes per row).
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, uploadMat.data);

    // Restore default alignment to avoid affecting other OpenGL operations
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::allocate(int width, int height, GLenum internalFormat, GLenum format) {
    m_width = width;
    m_height = height;
    m_internalFormat = internalFormat;
    m_dataFormat = format;

    glBindTexture(GL_TEXTURE_2D, m_rendererID);
    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_dataFormat, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}
