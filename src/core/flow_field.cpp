#include "flow_field.h"
#include <glm/gtc/noise.hpp>

/**
 * @brief Implementation of FlowField using GLM's Perlin noise.
 */
glm::vec2 FlowField::getForce(glm::vec2 pos, float time, float scale) {
    // Scale position helps to see the noise pattern better (making it less high-frequency on screen)
    // Adding time to z-coordinate animates the field
    float angle = glm::perlin(glm::vec3(pos.x * scale, pos.y * scale, time * 0.5f)) * 3.14159f * 4.0f; // Map -1..1 to angle
    
    // Create unit vector from angle
    return glm::vec2(cos(angle), sin(angle));
}
