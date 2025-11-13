#include <light.hpp>

Light::Light()
    : ambient(0.0f)
    , diffuse(0.0f)
    , specular(0.0f)
    , constant(0.0f)
    , linear(0.0f)
    , quadratic(0.0f)
    , depth_map_created(false)
    , depth_map(0) {}

Light::Light(float ambient, float diffuse, float specular, float constant, float linear,
             float quadratic)
    : ambient(ambient)
    , diffuse(diffuse)
    , specular(specular)
    , constant(constant)
    , linear(linear)
    , quadratic(quadratic)
    , depth_map_created(false)
    , depth_map(0) {}

void Light::createDepthMapTexture(const unsigned int shadow_width,
                                  const unsigned int shadow_height) {
    if (depth_map_created) {
        return;
    }

    glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_map);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadow_width,
                     shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    depth_map_created = true;
}
