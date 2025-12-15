#pragma once

#include <glm/glm.hpp>

/**
 * @brief Generates fluid-like forces using noise.
 */
class FlowField {
public:
    /**
     * @brief Calculates a directional force based on position and time.
     * 
     * @param pos Current position of the particle.
     * @param time Global time for animation.
     * @return glm::vec2 The force vector.
     */
    static glm::vec2 getForce(glm::vec2 pos, float time);
};
