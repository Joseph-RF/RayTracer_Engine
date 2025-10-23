#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in DATA {
    vec3 normal;
    mat4 projection;
} data_in[];

out vec3 colour;

const float scale_factor = 0.1f;

void generateLine(int index) {
    gl_Position = data_in[index].projection * gl_in[index].gl_Position;
    EmitVertex();

    gl_Position =
        data_in[index].projection * (gl_in[index].gl_Position + scale_factor * vec4(data_in[index].normal, 0.0));
    EmitVertex();

    EndPrimitive();
}

void main() {
    generateLine(0);
    generateLine(1);
    generateLine(2);
}