#include <arrow.hpp>

unsigned int Arrow::VBO;
unsigned int Arrow::EBO;
unsigned int Arrow::VAO;

int Arrow::num_sectors   = 20;
float Arrow::tail_radius = 0.02f;
float Arrow::tail_height = 1.0f;
float Arrow::head_radius = 0.1f;
float Arrow::head_height = 0.25f;

Arrow::Arrow() {
    this->pos         = glm::vec3(0.0, 0.0, 0.0);
    this->orientation = glm::vec3(0.0, 0.0, 0.0);
    this->scale       = glm::vec3(1.0, 1.0, 1.0);
    this->colour      = glm::vec3(1.0, 1.0, 1.0);
    this->shininess   = 0.0f;

    update_bounding_box();
    this->name  = "NO_NAME";
    this->light = nullptr;
}

Arrow::Arrow(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour,
             float shininess) {
    this->pos         = pos;
    this->orientation = orientation;
    this->scale       = scale;
    this->colour      = colour;
    this->shininess   = shininess;

    update_bounding_box();
    name  = "NO_NAME";
    light = nullptr;
}

void Arrow::draw(Shader& shader) {
    glBindVertexArray(Arrow::VAO);
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

    glDrawElements(GL_TRIANGLES, 12 * Arrow::num_sectors, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Arrow::update_bounding_box() {
    float radius                    = std::max(Arrow::head_radius, Arrow::tail_radius);
    std::vector<glm::vec3> vertices = {
        glm::vec3(-0.5f * radius, -0.5f * radius, -0.5f * tail_height),
        glm::vec3(0.5f * radius, -0.5f * radius, -0.5f * tail_height),
        glm::vec3(-0.5f * radius, 0.5f * radius, -0.5f * tail_height),
        glm::vec3(0.5f * radius, 0.5f * radius, -0.5f * tail_height),
        glm::vec3(-0.5f * radius, -0.5f * radius, head_height + 0.5f * tail_height),
        glm::vec3(0.5f * radius, -0.5f * radius, head_height + 0.5f * tail_height),
        glm::vec3(-0.5f * radius, 0.5f * radius, head_height + 0.5f * tail_height),
        glm::vec3(0.5f * radius, 0.5f * radius, head_height + 0.5f * tail_height)};

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

void Arrow::add_light(float ambient, float diffuse, float specular, float constant, float linear,
                      float quadratic) {
    this->light = std::make_unique<Light>(ambient, diffuse, specular, constant, linear, quadratic);
}

std::string Arrow::dataToString() {
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

std::vector<float> Arrow::unitCircleVertices() {
    // Return an array of the positions in 3D space of vertices around the edge of
    // a unit circle on the XY-plane

    std::vector<float> vertices;

    for (int i = 0; i < Arrow::num_sectors; ++i) {
        float theta = i * (2 * 3.141592) / Arrow::num_sectors;
        vertices.push_back(std::cos(theta)); // x
        vertices.push_back(std::sin(theta)); // y
        vertices.push_back(0.0f);            // z
    }

    return vertices;
}

void Arrow::init() {
    // Get the vertices of the cylinder from the circles on either end of the
    // cylinder

    std::vector<float> vertices;
    std::vector<float> unit_circles_vertices = unitCircleVertices();

    // Add vertices for tail cylinder base first (when i = 0) then top circle
    // (when i = 1)
    for (unsigned int i = 0; i < 2; ++i) {
        float z = (-0.5f * Arrow::tail_height) + i * Arrow::tail_height;

        for (unsigned int j = 0; j < unit_circles_vertices.size(); j += 3) {
            vertices.push_back(unit_circles_vertices[j] * Arrow::tail_radius);     // x
            vertices.push_back(unit_circles_vertices[j + 1] * Arrow::tail_radius); // y
            vertices.push_back(z);                                                 // z
        }
    }

    // Add the vertices for the arrow head
    for (unsigned int i = 0; i < 2; ++i) {
        float radius = (1 - i) * Arrow::head_radius;
        float z      = (0.5f * Arrow::tail_height) + i * Arrow::head_height;

        for (unsigned int j = 0; j < unit_circles_vertices.size(); j += 3) {
            vertices.push_back(unit_circles_vertices[j] * radius);     // x
            vertices.push_back(unit_circles_vertices[j + 1] * radius); // y
            vertices.push_back(z);
        }
    }

    // Vertices added, now need to add the indices for OpenGL to draw them in the
    // right order
    std::vector<unsigned int> indices;
    unsigned int k1 = 0;
    unsigned int k2 = Arrow::num_sectors;

    for (unsigned int i = 0; i < Arrow::num_sectors; ++i, ++k1, ++k2) {
        // k1 -> k1 + 1 -> k2 + 1
        indices.push_back(k1);
        indices.push_back((k1 + 1) % Arrow::num_sectors);
        indices.push_back(((k2 + 1) % Arrow::num_sectors) + Arrow::num_sectors);

        // k2 + 1 -> k2 -> k1
        indices.push_back(((k2 + 1) % Arrow::num_sectors) + Arrow::num_sectors);
        indices.push_back(k2);
        indices.push_back(k1);
    }

    unsigned int k3 = 2 * Arrow::num_sectors;
    unsigned int k4 = 3 * Arrow::num_sectors;

    for (unsigned int i = 0; i < Arrow::num_sectors; ++i, ++k3, ++k4) {
        // k3 -> k3 + 1 -> k4 + 1
        indices.push_back(k3);
        indices.push_back((k3 + 1) % Arrow::num_sectors + 2 * Arrow::num_sectors);
        indices.push_back(((k4 + 1) % Arrow::num_sectors) + 3 * Arrow::num_sectors);

        // k4 + 1 -> k4 -> k3
        indices.push_back(((k4 + 1) % Arrow::num_sectors) + 3 * Arrow::num_sectors);
        indices.push_back(k4);
        indices.push_back(k3);
    }

    glGenBuffers(1, &Arrow::EBO);
    glGenBuffers(1, &Arrow::VBO);
    glGenVertexArrays(1, &Arrow::VAO);

    glBindVertexArray(Arrow::VAO);

    glBindBuffer(GL_ARRAY_BUFFER, Arrow::VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Arrow::EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                 GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
