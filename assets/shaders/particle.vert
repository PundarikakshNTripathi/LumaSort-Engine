#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;

uniform float uPointSize;  // Dynamic point size based on viewport/simulation ratio
uniform vec2 uScale;       // Scale factors for aspect-ratio-preserving letterboxing

out vec4 vColor;

void main() {
    // Map 0..1 (Image Coords) to -1..1 (NDC)
    vec2 ndc = aPos * 2.0 - 1.0;
    
    // Flip Y so 0 is top (image coordinate convention)
    ndc.y = -ndc.y;
    
    // Apply scale for letterboxing (maintains aspect ratio)
    // uScale is < 1.0 on the axis that needs letterboxing
    ndc *= uScale;
    
    gl_Position = vec4(ndc, 0.0, 1.0);
    gl_PointSize = uPointSize;
    vColor = aColor;
}
