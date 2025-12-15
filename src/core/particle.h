#pragma once

#include <glm/glm.hpp>

/**
 * @brief Represents a single pixel rendered as a moving particle.
 */
struct Particle {
    glm::vec2 pos;      ///< Current 2D position (normalized 0..1 or screen coords)
    glm::vec2 vel;      ///< Current velocity vector
    glm::vec2 acc;      ///< Current acceleration vector
    glm::vec2 target;   ///< Target destination (sorted position)
    glm::vec4 color;    ///< RGBA color of the particle
};
