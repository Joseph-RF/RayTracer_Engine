#pragma once

#include <glad/glad.h>

#include <string>
#include <vector>

#include <shader.hpp>

namespace Skybox {
extern unsigned int VBO;
extern unsigned int VAO;
extern float brightness;

void draw(Shader& shader, unsigned int texture_id);

void init();
} // namespace Skybox
