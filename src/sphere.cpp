#include <sphere.hpp>

unsigned int Sphere::VBO;
unsigned int Sphere::EBO;
unsigned int Sphere::VAO;

unsigned int Sphere::stacks = 10;
unsigned int Sphere::slices = 10;

Sphere::Sphere() {
    this->pos         = glm::vec3(0.0, 0.0, 0.0);
    this->orientation = glm::vec3(0.0, 0.0, 0.0);
    this->scale       = glm::vec3(1.0, 1.0, 1.0);
    this->colour      = glm::vec3(1.0, 1.0, 1.0);
    this->shininess   = 32.0f;

    update_bounding_box();
    this->name    = "NO_NAME";
    this->light   = nullptr;
    this->visible = true;
}

Sphere::Sphere(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour,
               float shininess) {
    this->pos         = pos;
    this->orientation = orientation;
    this->scale       = scale;
    this->colour      = colour;
    this->shininess   = shininess;

    update_bounding_box();
    this->name    = "NO_NAME";
    this->light   = nullptr;
    this->visible = true;
}

void Sphere::draw(Shader& shader) {
    if (!visible) {
        return;
    }

    glBindVertexArray(Sphere::VAO);
    shader.use();

    glm::mat4 model(1.0);
    model = glm::translate(model, pos);
    model = glm::rotate(model, orientation.x, glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, orientation.y, glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, orientation.z, glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, scale);
    shader.setMat("model", model);
    shader.setVec3("colour", colour);
    shader.setFloat("shininess", shininess);

    // 3 Floats per vertex, 3 Vertices per triangle, (2 * (Stacks - 1) * Slices) triangle
    glDrawArrays(GL_TRIANGLES, 0, 3 * 3 * 2 * (Sphere::stacks - 1) * Sphere::slices);
    glBindVertexArray(0);
}

void Sphere::update_bounding_box() {
    return;
}

void Sphere::add_light(float ambient, float diffuse, float specular, float constant, float linear,
                       float quadratic) {
    this->light = std::make_unique<Light>(ambient, diffuse, specular, constant, linear, quadratic);
}

std::string Sphere::dataToString() {
    std::string str = "";

    // Game object type
    str += "Cube ";

    // Game object position
    str += (std::to_string(pos.x) + " ");
    str += (std::to_string(pos.y) + " ");
    str += (std::to_string(pos.z) + " ");

    // Game object orientation
    str += (std::to_string(orientation.x) + " ");
    str += (std::to_string(orientation.y) + " ");
    str += (std::to_string(orientation.z) + " ");

    // Game object scale
    str += (std::to_string(scale.x) + " ");
    str += (std::to_string(scale.y) + " ");
    str += (std::to_string(scale.z) + " ");

    // Game object colour
    str += (std::to_string(colour.x) + " ");
    str += (std::to_string(colour.y) + " ");
    str += (std::to_string(colour.z) + " ");

    // Game object shininess
    str += (std::to_string(shininess) + " ");

    // Game object name
    str += name;

    // Game object light
    if (light == nullptr) {
        str += " NOT_LIGHT ";
        str += "\n";
        return str;
    } else {
        str += " LIGHT ";
    }

    str += (std::to_string(light->ambient) + " ");
    str += (std::to_string(light->diffuse) + " ");
    str += (std::to_string(light->specular) + " ");

    str += (std::to_string(light->constant) + " ");
    str += (std::to_string(light->linear) + " ");
    str += (std::to_string(light->quadratic) + " ");

    str += "\n";

    return str;
}

void Sphere::init() {
    std::vector<float> vertices;

    for (unsigned int i = 0; i < Sphere::stacks; ++i) {
        float theta1 = i * 3.141592 / Sphere::stacks;
        float theta2 = (i + 1) * 3.141592 / Sphere::stacks;

        for (unsigned int j = 0; j < Sphere::slices; ++j) {
            float phi1 = j * 2 * 3.141592 / Sphere::slices;
            float phi2 = (j + 1) * 2 * 3.141592 / Sphere::slices;

            glm::vec3 vertex1(std::sin(theta1) * std::cos(phi1), std::cos(theta1),
                              std::sin(theta1) * std::sin(phi1));
            glm::vec3 vertex2(std::sin(theta2) * std::cos(phi1), std::cos(theta2),
                              std::sin(theta2) * std::sin(phi1));
            glm::vec3 vertex3(std::sin(theta2) * std::cos(phi2), std::cos(theta2),
                              std::sin(theta2) * std::sin(phi2));
            glm::vec3 vertex4(std::sin(theta1) * std::cos(phi2), std::cos(theta1),
                              std::sin(theta1) * std::sin(phi2));

            // Add the vertices for triangles

            if (i == 0) {
                // Top of the sphere
                vertices.push_back(vertex1.x);
                vertices.push_back(vertex1.y);
                vertices.push_back(vertex1.z);

                vertices.push_back(vertex2.x);
                vertices.push_back(vertex2.y);
                vertices.push_back(vertex2.z);

                vertices.push_back(vertex3.x);
                vertices.push_back(vertex3.y);
                vertices.push_back(vertex3.z);
            } else if (i == Sphere::stacks - 1) {
                vertices.push_back(vertex1.x);
                vertices.push_back(vertex1.y);
                vertices.push_back(vertex1.z);

                vertices.push_back(vertex2.x);
                vertices.push_back(vertex2.y);
                vertices.push_back(vertex2.z);

                vertices.push_back(vertex4.x);
                vertices.push_back(vertex4.y);
                vertices.push_back(vertex4.z);
            } else {
                vertices.push_back(vertex1.x);
                vertices.push_back(vertex1.y);
                vertices.push_back(vertex1.z);

                vertices.push_back(vertex2.x);
                vertices.push_back(vertex2.y);
                vertices.push_back(vertex2.z);

                vertices.push_back(vertex4.x);
                vertices.push_back(vertex4.y);
                vertices.push_back(vertex4.z);

                vertices.push_back(vertex2.x);
                vertices.push_back(vertex2.y);
                vertices.push_back(vertex2.z);

                vertices.push_back(vertex3.x);
                vertices.push_back(vertex3.y);
                vertices.push_back(vertex3.z);

                vertices.push_back(vertex4.x);
                vertices.push_back(vertex4.y);
                vertices.push_back(vertex4.z);
            }
        }
    }

    glGenBuffers(1, &Sphere::VBO);
    glGenVertexArrays(1, &Sphere::VAO);

    glBindVertexArray(Sphere::VAO);

    glBindBuffer(GL_ARRAY_BUFFER, Sphere::VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
