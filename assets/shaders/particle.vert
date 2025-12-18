#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;

uniform float uPointSize;  // Dynamic point size based on viewport/simulation ratio

out vec4 vColor;

void main() {
    // Map 0..1 (Image Coords) to -1..1 (NDC)
    // Flip Y so 0 is top
    gl_Position = vec4(aPos.x * 2.0 - 1.0, (1.0 - aPos.y) * 2.0 - 1.0, 0.0, 1.0);
    gl_PointSize = uPointSize;
    vColor = aColor;
}
