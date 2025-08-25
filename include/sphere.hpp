#pragma once

#include <vector>

#include <aabb.hpp>
#include <gameobject.hpp>
#include <shader.hpp>

class Sphere : public GameObject {
public:
    Sphere();

    Sphere(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour,
           float shininess);

    void draw(Shader& shader) override;

    void update_bounding_box() override;

    void add_light(float ambient, float diffuse, float specular, float constant, float linear,
                   float quadratic) override;

    std::string dataToString() override;

    static void init();

private:
    static unsigned int VBO;
    static unsigned int EBO;
    static unsigned int VAO;

    static unsigned int stacks;
    static unsigned int slices;
};
