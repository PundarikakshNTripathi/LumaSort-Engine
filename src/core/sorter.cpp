#include "sorter.h"
#include <algorithm>
#include <iostream>

Sorter::Sorter() {}

Sorter::~Sorter() {}

float Sorter::getLuminance(const cv::Vec3b& color) {
    // Perceived luminance: 0.299R + 0.587G + 0.114B
    // OpenCV uses BGR
    return 0.114f * color[0] + 0.587f * color[1] + 0.299f * color[2];
}

std::vector<glm::vec2> Sorter::sortImage(const cv::Mat& input, const cv::Mat& target, int simulationWidth, int simulationHeight) {
    std::vector<glm::vec2> mapping;
    
    if (input.empty() || target.empty()) {
        std::cerr << "Sorter::sortImage: Empty input or target!" << std::endl;
        return mapping;
    }

    // 1. Resize images to simulation grid
    cv::Mat resInput, resTarget;
    cv::resize(input, resInput, cv::Size(simulationWidth, simulationHeight));
    cv::resize(target, resTarget, cv::Size(simulationWidth, simulationHeight));

    // 2. Flatten and store PixelInfo
    int numPixels = simulationWidth * simulationHeight;
    std::vector<PixelInfo> inputPixels;
    std::vector<PixelInfo> targetPixels;
    inputPixels.reserve(numPixels);
    targetPixels.reserve(numPixels);

    for (int y = 0; y < simulationHeight; ++y) {
        for (int x = 0; x < simulationWidth; ++x) {
            // Input
            cv::Vec3b inColor = resInput.at<cv::Vec3b>(y, x);
            inputPixels.push_back({ getLuminance(inColor), x, y });

            // Target
            cv::Vec3b tgtColor = resTarget.at<cv::Vec3b>(y, x);
            targetPixels.push_back({ getLuminance(tgtColor), x, y });
        }
    }

    // 3. Sort both arrays based on luminance
    auto comparator = [](const PixelInfo& a, const PixelInfo& b) {
        return a.luminance < b.luminance;
    };

    std::sort(inputPixels.begin(), inputPixels.end(), comparator);
    std::sort(targetPixels.begin(), targetPixels.end(), comparator);

    // 4. Create Mapping
    // inputPixels[k] corresponds to targetPixels[k]
    // mapping[original_input_index] = target_pos
    
    // Resize mapping to size needed
    mapping.resize(numPixels);

    for (int k = 0; k < numPixels; ++k) {
        const PixelInfo& inPix = inputPixels[k];
        const PixelInfo& tgtPix = targetPixels[k];

        // Original index in the flattened array
        int originalIndex = inPix.original_y * simulationWidth + inPix.original_x;

        // Assign the target position
        mapping[originalIndex] = glm::vec2(tgtPix.original_x, tgtPix.original_y);
    }

    return mapping;
}
