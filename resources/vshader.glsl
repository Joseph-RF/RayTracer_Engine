// Note: This vertex shader doesn't do much, just forwarded input to output
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 frag_pos;

void main() {
    normal = mat3(transpose(inverse(model))) * aNormal;

    // Frag position needs to be in world coordinates
    frag_pos = vec3(model * vec4(aPos, 1.0));

    gl_Position = projection * view * model * vec4(aPos, 1.0); // Cast aPos to a vec4
};