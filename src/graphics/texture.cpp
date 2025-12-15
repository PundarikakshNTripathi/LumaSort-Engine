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

void Texture2D::uploadFromOpenCV(const cv::Mat& mat) {
    if (mat.empty()) {
        std::cerr << "Texture2D::uploadFromOpenCV: Empty matrix provided!" << std::endl;
        return;
    }

    m_width = mat.cols;
    m_height = mat.rows;

    GLenum format = GL_RGB;
    if (mat.channels() == 4) {
        format = GL_RGBA;
        m_internalFormat = GL_RGBA8;
    } else if (mat.channels() == 3) {
        format = GL_BGR; // OpenCV uses BGR by default
        m_internalFormat = GL_RGB8;
    } else if (mat.channels() == 1) {
        format = GL_RED;
        m_internalFormat = GL_R8;
    }

    m_dataFormat = format;

    glBindTexture(GL_TEXTURE_2D, m_rendererID);
    
    // Pixel alignment for OpenCV data (rows are 4-byte aligned by default implementation, 
    // but sometimes tight packing is needed, though default usually works for 3/4 channels. 
    // For 1 channel or odd widths, glPixelStorei might be needed.)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, mat.data);
    
    // Reset alignment to default
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Generate mipmaps if needed, but for video feed it's usually overkill/slow every frame. 
    // We kept min filter linear so no mipmaps needed.
    
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
