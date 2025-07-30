// Note: This vertex shader doesn't do much, just forwarded input to output
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0); // Cast aPos to a vec4
};