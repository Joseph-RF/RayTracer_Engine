#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

layout(std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};
uniform mat4 model;

out DATA {
    vec3 normal;
    mat4 projection;
} data_out;

void main() {
    data_out.normal = normalize(vec3(vec4(mat3(transpose(inverse(view * model))) * aNormal, 0.0)));
    data_out.projection = projection;

    // Note, not mulitplying by projection matrix. That will be done in geometry shader
    gl_Position = view * model * vec4(aPos, 1.0);
};