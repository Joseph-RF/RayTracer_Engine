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

    this->name    = "NO_NAME";
    this->light   = nullptr;
    this->visible = true;
}

Arrow::Arrow(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour,
             float shininess) {
    this->pos         = pos;
    this->orientation = orientation;
    this->scale       = scale;
    this->colour      = colour;
    this->shininess   = shininess;

    name          = "NO_NAME";
    light         = nullptr;
    this->visible = true;
}

void Arrow::draw(Shader& shader) {
    if (!visible) {
        return;
    }

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

    // 3 vertices per triangle, 2 triangles per sector, 2 cylinders plus 2 circles per arrow
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * (2 + 2) * Arrow::num_sectors);
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

void Arrow::add_light(float ambient, float diffuse, float specular, float constant, float linear,
                      float quadratic) {
    this->light = std::make_unique<Light>(ambient, diffuse, specular, constant, linear, quadratic);
}

std::string Arrow::dataToString() {
    // Start with the object type
    std::string str = "Arrow";

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
        float theta = i * (2.f * 3.141592f) / Arrow::num_sectors;
        vertices.push_back(std::cos(theta)); // x
        vertices.push_back(std::sin(theta)); // y
        vertices.push_back(0.0f);            // z
    }

    return vertices;
}

std::vector<float> Arrow::generateVertexPositions() {
    // Get the vertices of the cylinder from the circles on either end of the
    // cylinder

    std::vector<float> vertex_positions;
    std::vector<float> unit_circles_vertices = unitCircleVertices();
    unsigned int n                           = unit_circles_vertices.size();

    float z = 0.0;

    // Add the vertex positions for triangles with their base along the bottom
    // of the cylinder
    z = -0.5f * Arrow::tail_height;

    for (unsigned int i = 0; i < unit_circles_vertices.size(); i += 3) {
        // Vertex 1
        vertex_positions.push_back(unit_circles_vertices[i] * Arrow::tail_radius);     // x
        vertex_positions.push_back(unit_circles_vertices[i + 1] * Arrow::tail_radius); // y
        vertex_positions.push_back(z);                                                 // z

        // Vertex 2, same height as vertex 1 just one sector along
        vertex_positions.push_back(unit_circles_vertices[(i + 3) % n] * Arrow::tail_radius); // x
        vertex_positions.push_back(unit_circles_vertices[(i + 4) % n] * Arrow::tail_radius); // y
        vertex_positions.push_back(z);                                                       // z

        // Vertex 3, same sector as vertex 2 but at the above circle
        vertex_positions.push_back(unit_circles_vertices[(i + 3) % n] * Arrow::tail_radius); // x
        vertex_positions.push_back(unit_circles_vertices[(i + 4) % n] * Arrow::tail_radius); // y
        vertex_positions.push_back(z + Arrow::tail_height);                                  // z
    }

    // Add vertex positions for the triangles with their base along the top of the
    // cylinder
    for (unsigned int i = 0; i < unit_circles_vertices.size(); i += 3) {
        // Vertex 1
        vertex_positions.push_back(unit_circles_vertices[i] * Arrow::tail_radius);     // x
        vertex_positions.push_back(unit_circles_vertices[i + 1] * Arrow::tail_radius); // y
        vertex_positions.push_back(z + Arrow::tail_height);                            // z

        // Vertex 2, same sector as vertex 1 but at the lower circle
        vertex_positions.push_back(unit_circles_vertices[i] * Arrow::tail_radius);     // x
        vertex_positions.push_back(unit_circles_vertices[i + 1] * Arrow::tail_radius); // y
        vertex_positions.push_back(z);                                                 // z

        // Vertex 3, same height as vertex 1 just one sector along
        vertex_positions.push_back(unit_circles_vertices[(i + 3) % n] * Arrow::tail_radius); // x
        vertex_positions.push_back(unit_circles_vertices[(i + 4) % n] * Arrow::tail_radius); // y
        vertex_positions.push_back(z + Arrow::tail_height);                                  // z
    }

    // Add triangles for the circle at the base of the arrow tail
    for (unsigned int i = 0; i < unit_circles_vertices.size(); i += 3) {
        // Vertex 1
        vertex_positions.push_back(unit_circles_vertices[i] * Arrow::tail_radius);     // x
        vertex_positions.push_back(unit_circles_vertices[i + 1] * Arrow::tail_radius); // y
        vertex_positions.push_back(z);                                                 // z

        // Vertex 2
        vertex_positions.push_back(0.0f); // x
        vertex_positions.push_back(0.0f); // y
        vertex_positions.push_back(z);    // z

        // Vertex 3
        vertex_positions.push_back(unit_circles_vertices[(i + 3) % n] * Arrow::tail_radius); // x
        vertex_positions.push_back(unit_circles_vertices[(i + 4) % n] * Arrow::tail_radius); // y
        vertex_positions.push_back(z);                                                       // z
    }

    // Repeat the two loops above but for the arrow head
    // Add the vertex positions for triangles with their base along the bottom
    // of the arrow head
    z = 0.5f * Arrow::tail_height;

    for (unsigned int i = 0; i < unit_circles_vertices.size(); i += 3) {
        // Vertex 1
        vertex_positions.push_back(unit_circles_vertices[i] * Arrow::head_radius);     // x
        vertex_positions.push_back(unit_circles_vertices[i + 1] * Arrow::head_radius); // y
        vertex_positions.push_back(z);                                                 // z

        // Vertex 2, same height as vertex 1 just one sector along
        vertex_positions.push_back(unit_circles_vertices[(i + 3) % n] * Arrow::head_radius); // x
        vertex_positions.push_back(unit_circles_vertices[(i + 4) % n] * Arrow::head_radius); // y
        vertex_positions.push_back(z);                                                       // z

        // Vertex 3, same sector as vertex 2 but at tip of the arrow head
        vertex_positions.push_back(0.0f);                   // x
        vertex_positions.push_back(0.0f);                   // y
        vertex_positions.push_back(z + Arrow::head_height); // z
    }

    // Add vertex positions for the triangles with their base along the top of the
    // arrow head
    for (unsigned int i = 0; i < unit_circles_vertices.size(); i += 3) {
        // Vertex 1
        vertex_positions.push_back(0.0f);                   // x
        vertex_positions.push_back(0.0f);                   // y
        vertex_positions.push_back(z + Arrow::head_height); // z

        // Vertex 2, same sector as vertex 1 but at the lower circle
        vertex_positions.push_back(unit_circles_vertices[i] * Arrow::head_radius);     // x
        vertex_positions.push_back(unit_circles_vertices[i + 1] * Arrow::head_radius); // y
        vertex_positions.push_back(z);                                                 // z

        // Vertex 3, same height as vertex 1 just one sector along
        vertex_positions.push_back(0.0f);                   // x
        vertex_positions.push_back(0.0f);                   // y
        vertex_positions.push_back(z + Arrow::head_height); // z
    }

    // Add triangles for the circle at the base of the arrow head
    for (unsigned int i = 0; i < unit_circles_vertices.size(); i += 3) {
        // Vertex 1
        vertex_positions.push_back(unit_circles_vertices[i] * Arrow::head_radius);     // x
        vertex_positions.push_back(unit_circles_vertices[i + 1] * Arrow::head_radius); // y
        vertex_positions.push_back(z);                                                 // z

        // Vertex 2
        vertex_positions.push_back(0.0f); // x
        vertex_positions.push_back(0.0f); // y
        vertex_positions.push_back(z);    // z

        // Vertex 3
        vertex_positions.push_back(unit_circles_vertices[(i + 3) % n] * Arrow::head_radius); // x
        vertex_positions.push_back(unit_circles_vertices[(i + 4) % n] * Arrow::head_radius); // y
        vertex_positions.push_back(z);                                                       // z
    }

    return vertex_positions;
}

std::vector<float> Arrow::generateVertexNormals(const std::vector<float>& vertex_positions) {
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

std::vector<unsigned int> Arrow::generateIndices() {
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

    return indices;
}

void Arrow::init() {
    std::vector<float> vertex_positions = generateVertexPositions();
    std::vector<float> vertex_normals   = generateVertexNormals(vertex_positions);

    // Vertices added, now need to add the indices for OpenGL to draw them in the
    // right order
    std::vector<unsigned int> indices = generateIndices();

    // glGenBuffers(1, &Arrow::EBO);
    glGenBuffers(1, &Arrow::VBO);
    glGenVertexArrays(1, &Arrow::VAO);

    glBindVertexArray(Arrow::VAO);

    glBindBuffer(GL_ARRAY_BUFFER, Arrow::VBO);
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

std::shared_ptr<Arrow> createArrowFromData(const std::string& data) {
    std::stringstream ss(data);
    std::string str;

    std::shared_ptr<Arrow> temp =
        std::make_shared<Arrow>(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0),
                                glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 0.0, 0.0), 85.0f);

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
