#pragma once

#include <vector>

#include <aabb.hpp>
#include <gameobject.hpp>
#include <shader.hpp>

class Arrow : public GameObject {
public:
    Arrow();

    Arrow(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour, float shininess);

    void draw(Shader& shader) override;

    void update_bounding_box() override;

    void add_light(float ambient, float diffuse, float specular, float constant, float linear,
                   float quadratic) override;

    std::string dataToString() override;

    static void init();

    static float tail_radius;
    static float tail_height;
    static float head_radius;
    static float head_height;

private:
    static std::vector<float> unitCircleVertices();

    static std::vector<float> generateVertexPositions();

    static std::vector<float> generateVertexNormals(const std::vector<float>& vertex_positions);

    static std::vector<unsigned int> generateIndices();

    static unsigned int VBO;
    static unsigned int EBO;
    static unsigned int VAO;

    static int num_sectors;
};

std::shared_ptr<Arrow> createArrowFromData(const std::string& data);
