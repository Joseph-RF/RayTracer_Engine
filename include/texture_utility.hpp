#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <stb_image.h>

namespace TextureUtility {
unsigned int loadCubeMapTexture(std::vector<std::string> faces);
} // namespace TextureUtility
