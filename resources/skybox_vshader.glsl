#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

// Texture dir should be equivalent to texture coords as long as cube is placed at the origin
out vec3 texture_dir;

void main() {
	texture_dir = aPos;
	gl_Position = projection * view * vec4(aPos, 1.0); // Cast aPos to a vec4
}