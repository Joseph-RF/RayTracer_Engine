#version 330 core

uniform samplerCube cubemap;
uniform float brightness;

in vec3 texture_dir; // Direction from origin pointing at skybox

out vec4 FragColor;

void main() {
	FragColor = brightness * texture(cubemap, texture_dir);
}