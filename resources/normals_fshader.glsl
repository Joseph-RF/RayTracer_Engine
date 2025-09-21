#version 330 core

in vec3 colour;

out vec4 FragColor; // Fragment shader only requires a single output variable

void main() {
    FragColor = vec4(1.0, 1.0, 0.0, 1.0); // Output must be vec4
};