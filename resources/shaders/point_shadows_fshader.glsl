#version 330 core

in vec4 FragPos;

uniform vec3 light_pos;
uniform float far_plane;

void main() {
	// Distance from fragment to light source in world-space
    float light_distance = length(FragPos.xyz - light_pos);

	// Map to [0, 1] range by dividing by far plane
    light_distance = light_distance / far_plane;

	gl_FragDepth = light_distance;
}