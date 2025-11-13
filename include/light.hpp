#pragma once

#include <glad/glad.h>

class Light {
public:
    Light();

    Light(float ambient, float diffuse, float specular, float constant, float linear,
          float quadratic);

    void createDepthMapTexture(unsigned int shadow_width, unsigned int shadow_height);

    float ambient;
    float diffuse;
    float specular;
    float constant;
    float linear;
    float quadratic;

    bool depth_map_created;
    unsigned int depth_map;
};
