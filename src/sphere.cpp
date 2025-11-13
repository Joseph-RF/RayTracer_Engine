#include "sphere.hpp"
#include <sphere.hpp>

unsigned int Sphere::VBO;
unsigned int Sphere::EBO;
unsigned int Sphere::VAO;

unsigned int Sphere::stacks = 50;
unsigned int Sphere::slices = 50;

Sphere::Sphere() {
    this->pos         = glm::vec3(0.0, 0.0, 0.0);
    this->orientation = glm::vec3(0.0, 0.0, 0.0);
    this->scale       = glm::vec3(1.0, 1.0, 1.0);
    this->colour      = glm::vec3(1.0, 1.0, 1.0);
    this->shininess   = 32.0f;

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

    // 3 vertices per triangle, (2 * (Stacks - 1) * Slices) triangle
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * (Sphere::stacks - 1) * Sphere::slices);
    glBindVertexArray(0);
}

void Sphere::update_bounding_box() {
    std::vector<glm::vec3> vertices = {
        glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f),  glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),  glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),   glm::vec3(1.0f, 1.0f, 1.0f)};

    for (glm::vec3& vertex : vertices) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);
        model = glm::rotate(model, orientation.x, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, orientation.y, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, orientation.z, glm::vec3(0.0, 0.0, 1.0));
        model = glm::scale(model, scale);

        vertex = glm::vec3(model * glm::vec4(vertex, 1.0));
    }

    // Default bbox given the cube's vertices
    // Adjust depending on the transformations
    // NOTE: Assume that it won't change later
    bbox = AABB(vertices[0].x, vertices[0].x, vertices[0].y, vertices[0].y, vertices[0].z,
                vertices[0].z);

    for (unsigned int i = 1; i < vertices.size(); ++i) {
        if (vertices[i].x < bbox.xmin) {
            bbox.xmin = vertices[i].x;
        }
        if (vertices[i].x > bbox.xmax) {
            bbox.xmax = vertices[i].x;
        }
        if (vertices[i].y < bbox.ymin) {
            bbox.ymin = vertices[i].y;
        }
        if (vertices[i].y > bbox.ymax) {
            bbox.ymax = vertices[i].y;
        }
        if (vertices[i].z < bbox.zmin) {
            bbox.zmin = vertices[i].z;
        }
        if (vertices[i].z > bbox.zmax) {
            bbox.zmax = vertices[i].z;
        }
    }
}

void Sphere::add_light(float ambient, float diffuse, float specular, float constant, float linear,
                       float quadratic) {
    this->light = std::make_unique<Light>(ambient, diffuse, specular, constant, linear, quadratic);
}

std::string Sphere::dataToString() {
    std::string str = "";

    // Game object type
    str += "Sphere ";

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

std::vector<float> Sphere::generateVertexPositions() {

    std::vector<float> vertex_positions;

    for (unsigned int i = 0; i < Sphere::stacks; ++i) {
        float theta1 = i * 3.141592f / Sphere::stacks;
        float theta2 = (i + 1) * 3.141592f / Sphere::stacks;

        for (unsigned int j = 0; j < Sphere::slices; ++j) {
            float phi1 = j * 2 * 3.141592f / Sphere::slices;
            float phi2 = (j + 1) * 2 * 3.141592f / Sphere::slices;

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
                vertex_positions.push_back(vertex1.x);
                vertex_positions.push_back(vertex1.y);
                vertex_positions.push_back(vertex1.z);

                vertex_positions.push_back(vertex3.x);
                vertex_positions.push_back(vertex3.y);
                vertex_positions.push_back(vertex3.z);

                vertex_positions.push_back(vertex2.x);
                vertex_positions.push_back(vertex2.y);
                vertex_positions.push_back(vertex2.z);
            } else if (i == Sphere::stacks - 1) {
                // Bottom of the sphere
                vertex_positions.push_back(vertex1.x);
                vertex_positions.push_back(vertex1.y);
                vertex_positions.push_back(vertex1.z);

                vertex_positions.push_back(vertex4.x);
                vertex_positions.push_back(vertex4.y);
                vertex_positions.push_back(vertex4.z);

                vertex_positions.push_back(vertex2.x);
                vertex_positions.push_back(vertex2.y);
                vertex_positions.push_back(vertex2.z);
            } else {
                // Middle section of sphere
                vertex_positions.push_back(vertex1.x);
                vertex_positions.push_back(vertex1.y);
                vertex_positions.push_back(vertex1.z);

                vertex_positions.push_back(vertex4.x);
                vertex_positions.push_back(vertex4.y);
                vertex_positions.push_back(vertex4.z);

                vertex_positions.push_back(vertex2.x);
                vertex_positions.push_back(vertex2.y);
                vertex_positions.push_back(vertex2.z);

                vertex_positions.push_back(vertex2.x);
                vertex_positions.push_back(vertex2.y);
                vertex_positions.push_back(vertex2.z);

                vertex_positions.push_back(vertex4.x);
                vertex_positions.push_back(vertex4.y);
                vertex_positions.push_back(vertex4.z);

                vertex_positions.push_back(vertex3.x);
                vertex_positions.push_back(vertex3.y);
                vertex_positions.push_back(vertex3.z);
            }
        }
    }
    return vertex_positions;
}

std::vector<float> Sphere::generateVertexNormals(const std::vector<float>& vertex_positions) {
    std::vector<float> vertex_normals;
    for (unsigned int i = 0; i < vertex_positions.size(); i += 9) {
        glm::vec3 v1(vertex_positions[i], vertex_positions[i + 1], vertex_positions[i + 2]);
        glm::vec3 v2(vertex_positions[i + 3], vertex_positions[i + 4], vertex_positions[i + 5]);
        glm::vec3 v3(vertex_positions[i + 6], vertex_positions[i + 7], vertex_positions[i + 8]);

        glm::vec3 u = v2 - v1;
        glm::vec3 v = v3 - v1;

        glm::vec3 n = glm::cross(u, v);

        // Normal for v1
        vertex_normals.push_back(n.x);
        vertex_normals.push_back(n.y);
        vertex_normals.push_back(n.z);

        // Normal for v2
        vertex_normals.push_back(n.x);
        vertex_normals.push_back(n.y);
        vertex_normals.push_back(n.z);

        // Normal for v3
        vertex_normals.push_back(n.x);
        vertex_normals.push_back(n.y);
        vertex_normals.push_back(n.z);
    }
    return vertex_normals;
}

void Sphere::init() {
    std::vector<float> vertex_positions = generateVertexPositions();
    std::vector<float> vertex_normals   = generateVertexNormals(vertex_positions);

    glGenBuffers(1, &Sphere::VBO);
    glGenVertexArrays(1, &Sphere::VAO);

    glBindVertexArray(Sphere::VAO);

    glBindBuffer(GL_ARRAY_BUFFER, Sphere::VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (vertex_positions.size() + vertex_normals.size()),
                 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertex_positions.size(),
                    vertex_positions.data());
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * vertex_positions.size(),
                    sizeof(float) * vertex_normals.size(), vertex_normals.data());

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)(sizeof(float) * vertex_positions.size()));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

std::shared_ptr<Sphere> createSphereFromData(std::string& data) {
    std::stringstream ss(data);
    std::string str;

    std::shared_ptr<Sphere> temp =
        std::make_shared<Sphere>(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0),
                                 glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 0.0, 0.0), 32.0f);

    str = "";
    ss >> str; // Get rid of the number
    ss >> str; // Get rid of object class type

    // Cube position
    ss >> str;
    temp->pos.x = std::stof(str);
    ss >> str;
    temp->pos.y = std::stof(str);
    ss >> str;
    temp->pos.z = std::stof(str);

    // Cube orientation
    ss >> str;
    temp->orientation.x = std::stof(str);
    ss >> str;
    temp->orientation.y = std::stof(str);
    ss >> str;
    temp->orientation.z = std::stof(str);

    // Cube scale
    ss >> str;
    temp->scale.x = std::stof(str);
    ss >> str;
    temp->scale.y = std::stof(str);
    ss >> str;
    temp->scale.z = std::stof(str);

    // Cube colour
    ss >> str;
    temp->colour.x = std::stof(str);
    ss >> str;
    temp->colour.y = std::stof(str);
    ss >> str;
    temp->colour.z = std::stof(str);

    // Cube shininess
    ss >> str;
    temp->shininess = std::stof(str);

    // Cube name
    ss >> str;
    temp->name = str;

    // Update bounding box
    temp->update_bounding_box();

    ss >> str;
    if (str == "NOT_LIGHT") {
        temp->light = nullptr;
    } else {
        Light temp_light;

        // Light ambient
        ss >> str;
        temp_light.ambient = std::stof(str);
        // Light diffuse
        ss >> str;
        temp_light.diffuse = std::stof(str);
        // Light specular
        ss >> str;
        temp_light.specular = std::stof(str);

        // Light constant
        ss >> str;
        temp_light.constant = std::stof(str);
        // Light linear
        ss >> str;
        temp_light.linear = std::stof(str);
        // Light quadratic
        ss >> str;
        temp_light.quadratic = std::stof(str);

        temp->light =
            std::make_unique<Light>(temp_light.ambient, temp_light.diffuse, temp_light.specular,
                                    temp_light.constant, temp_light.linear, temp_light.quadratic);
    }
    return temp;
}
