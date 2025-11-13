#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 shadow_matrices[6]; // Light space transforms

// Fragment position emitted from geometry shader. Emitted per emitvertex()
// FragPos will be in world-space coordinates. Used to calculate depth in fragment shader
out vec4 FragPos;

void main() {
    for (int face = 0; face < 6; ++face) {
        gl_Layer = face; // Built-in variable that specifies cube map face
        for (int i = 0; i < 3; ++i) {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadow_matrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}