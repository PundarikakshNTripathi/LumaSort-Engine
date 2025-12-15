#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <glm/glm.hpp>

/**
 * @struct PixelInfo
 * @brief Holds luminance and original position of a pixel.
 */
struct PixelInfo {
    float luminance;
    int original_x;
    int original_y;
};

/**
 * @class Sorter
 * @brief Core logic for the Pixel Sorting algorithm.
 * 
 * Analyzes input and target images to create a mapping based on luminance.
 */
class Sorter {
public:
    Sorter();
    ~Sorter();

    /**
     * @brief Sorts pixels from input and target based on luminance and generates a mapping.
     * 
     * @param input The source image (Webcam/video/drawing).
     * @param target The destination "bunch of pixels" image (e.g. Portrait).
     * @param simulationWidth Width of the simulation grid.
     * @param simulationHeight Height of the simulation grid.
     * @return std::vector<glm::vec2> A mapping table where index = input pixel index, value = target position.
     */
    std::vector<glm::vec2> sortImage(const cv::Mat& input, const cv::Mat& target, int simulationWidth = 256, int simulationHeight = 256);

private:
    /**
     * @brief Helper to calculate luminance of a pixel.
     */
    float getLuminance(const cv::Vec3b& color);
};
