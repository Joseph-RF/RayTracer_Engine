#include <hollow_cylinder.hpp>

unsigned int HollowCylinder::VBO;
unsigned int HollowCylinder::VAO;
unsigned int HollowCylinder::EBO;

int HollowCylinder::num_sectors = 40;
float HollowCylinder::thickness = 0.01f;

HollowCylinder::HollowCylinder() {
    this->pos         = glm::vec3(0.0, 0.0, 0.0);
    this->orientation = glm::vec3(0.0, 0.0, 0.0);
    this->scale       = glm::vec3(1.0, 1.0, 1.0);
    this->colour      = glm::vec3(1.0, 1.0, 1.0);
    this->shininess   = 0.0f;

    update_bounding_box();
    this->name    = "NO_NAME";
    this->light   = nullptr;
    this->visible = true;
}

HollowCylinder::HollowCylinder(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale,
                               glm::vec3 colour, float shininess) {
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

void HollowCylinder::draw(Shader& shader) {
    if (!visible) {
        return;
    }

    glBindVertexArray(HollowCylinder::VAO);
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

    // 3 vertices per triangle, 8 * num_sector triangles
    glDrawElements(GL_TRIANGLES, 3 * 8 * HollowCylinder::num_sectors, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void HollowCylinder::update_bounding_box() {
    float radius                    = 2.0f;
    std::vector<glm::vec3> vertices = {glm::vec3(-0.5f * radius, -0.5f * radius, -0.5f),
                                       glm::vec3(0.5f * radius, -0.5f * radius, -0.5f),
                                       glm::vec3(-0.5f * radius, 0.5f * radius, -0.5f),
                                       glm::vec3(0.5f * radius, 0.5f * radius, -0.5f),
                                       glm::vec3(-0.5f * radius, -0.5f * radius, 0.5f),
                                       glm::vec3(0.5f * radius, -0.5f * radius, 0.5f),
                                       glm::vec3(-0.5f * radius, 0.5f * radius, 0.5f),
                                       glm::vec3(0.5f * radius, 0.5f * radius, 0.5f)};

    for (unsigned int i = 0; i < vertices.size(); ++i) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);
        model = glm::rotate(model, orientation.x, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, orientation.y, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, orientation.z, glm::vec3(0.0, 0.0, 1.0));
        model = glm::scale(model, scale);

        vertices[i] = glm::vec3(model * glm::vec4(vertices[i], 1.0));
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

void HollowCylinder::add_light(float ambient, float diffuse, float specular, float constant,
                               float linear, float quadratic) {
    this->light = std::make_unique<Light>(ambient, diffuse, specular, constant, linear, quadratic);
}

std::string HollowCylinder::dataToString() {
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

std::vector<float> HollowCylinder::unitCircleVertices() {
    // Return an array of the positions in 3D space of vertices around the edge of
    // a unit circle on the XY-plane

    std::vector<float> vertices;

    for (int i = 0; i < HollowCylinder::num_sectors; ++i) {
        float theta = i * (2 * 3.141592) / HollowCylinder::num_sectors;
        vertices.push_back(std::cos(theta)); // x
        vertices.push_back(std::sin(theta)); // y
        vertices.push_back(0.0f);            // z
    }

    return vertices;
}

void HollowCylinder::init() {
    // Get the vertices of the hollow cylinder from the circles on either end of the
    // cylinder

    std::vector<float> vertices;
    std::vector<float> unit_circles_vertices = HollowCylinder::unitCircleVertices();

    // Add vertices for tail cylinder base first (when i = 0) then top circle
    // (when i = 1)
    for (unsigned int i = 0; i < 2; ++i) {
        float z = -0.5f + i * 0.5f;

        for (unsigned int j = 0; j < unit_circles_vertices.size(); j += 3) {
            // Outer circle first
            vertices.push_back(unit_circles_vertices[j]);     // x
            vertices.push_back(unit_circles_vertices[j + 1]); // y
            vertices.push_back(z);                            // z

            // Inner circle next
            vertices.push_back(unit_circles_vertices[j] * (1 - HollowCylinder::thickness));     // x
            vertices.push_back(unit_circles_vertices[j + 1] * (1 - HollowCylinder::thickness)); // y
            vertices.push_back(z);                                                              // z
        }
    }

    // Vertices added, now need to add the indices for OpenGL to draw them in the
    // right order
    std::vector<unsigned int> indices;
    unsigned int sector          = 0;
    unsigned int vertical_offset = 2 * HollowCylinder::num_sectors; // Upper circle indices

    for (unsigned int i = 0; i < HollowCylinder::num_sectors; ++i, sector += 2) {

        // Exterior
        // k1 -> k1 + 2 -> k2 + 2
        indices.push_back(sector);
        indices.push_back((sector + 2) % vertical_offset);
        indices.push_back(((sector + vertical_offset + 2) % vertical_offset) + vertical_offset);

        // k2 + 2 -> k2 -> k1
        indices.push_back(((sector + vertical_offset + 2) % vertical_offset) + vertical_offset);
        indices.push_back(sector + vertical_offset);
        indices.push_back(sector);

        // Interior
        // k1 + 1 -> k1 + 3 -> k2 + 3
        indices.push_back((sector + 1));
        indices.push_back((sector + 3) % vertical_offset);
        indices.push_back(((sector + vertical_offset + 3) % vertical_offset) + vertical_offset);

        // k2 + 3 -> k2 + 1 -> k1 + 1
        indices.push_back(((sector + vertical_offset + 3) % vertical_offset) + vertical_offset);
        indices.push_back(((sector + vertical_offset + 1) % vertical_offset) + vertical_offset);
        indices.push_back((sector + 1));

        // Lower ring
        // k1 + 1 -> k1 -> k1 + 3
        indices.push_back((sector + 1));
        indices.push_back(sector);
        indices.push_back((sector + 3) % vertical_offset);

        // k1 -> k1 + 2 -> k1 + 3
        indices.push_back(sector);
        indices.push_back((sector + 2) % vertical_offset);
        indices.push_back((sector + 3) % vertical_offset);

        // Upper ring
        // k2 + 1 -> k2 -> k2 + 3
        indices.push_back(((sector + vertical_offset + 1) % vertical_offset) + vertical_offset);
        indices.push_back(sector + vertical_offset);
        indices.push_back(((sector + vertical_offset + 3) % vertical_offset) + vertical_offset);

        // k2 -> k2 + 2 -> k2 + 3
        indices.push_back(sector + vertical_offset);
        indices.push_back(((sector + vertical_offset + 2) % vertical_offset) + vertical_offset);
        indices.push_back(((sector + vertical_offset + 3) % vertical_offset) + vertical_offset);
    }

    glGenBuffers(1, &HollowCylinder::EBO);
    glGenBuffers(1, &HollowCylinder::VBO);
    glGenVertexArrays(1, &HollowCylinder::VAO);

    glBindVertexArray(HollowCylinder::VAO);

    glBindBuffer(GL_ARRAY_BUFFER, HollowCylinder::VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HollowCylinder::EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                 GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
