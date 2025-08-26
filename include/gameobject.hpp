#pragma once

#include <memory>
#include <string>

#include <aabb.hpp>
#include <light.hpp>
#include <shader.hpp>

class GameObject {
public:
    glm::vec3 pos;
    glm::vec3 orientation;
    glm::vec3 scale;
    glm::vec3 colour;
    float shininess;

    AABB bbox;
    std::string name;

    std::unique_ptr<Light> light;

    bool visible;

    virtual void draw(Shader& shader)                     = 0;
    virtual void update_bounding_box()                    = 0;
    virtual void add_light(float ambient, float diffuse, float specular, float constant,
                           float linear, float quadratic) = 0;

    virtual std::string dataToString() = 0;

private:
};
