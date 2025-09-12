#include <hollow_cylinder.hpp>

unsigned int HollowCylinder::VBO;
unsigned int HollowCylinder::VAO;
unsigned int HollowCylinder::EBO;

int HollowCylinder::num_sectors = 50;
float HollowCylinder::thickness = 0.1f;

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
    glDrawArrays(GL_TRIANGLES, 0, 3 * 8 * HollowCylinder::num_sectors);
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
    str += "HollowCylinder ";

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

std::vector<float> HollowCylinder::generateVertexPositions() {

    std::vector<float> vertex_positions;
    std::vector<float> unit_circles_vertices = HollowCylinder::unitCircleVertices();
    unsigned int n                           = unit_circles_vertices.size();

    float z = -0.5f;

    // Add the vertex positions for triangles on the outside of the cylinder
    for (unsigned int i = 0; i < unit_circles_vertices.size(); i += 3) {

        glm::vec3 v1(unit_circles_vertices[i], unit_circles_vertices[i + 1], z);
        glm::vec3 v2(unit_circles_vertices[(i + 3) % n], unit_circles_vertices[(i + 4) % n], z);
        glm::vec3 v3(unit_circles_vertices[i], unit_circles_vertices[i + 1], z + 1);
        glm::vec3 v4(unit_circles_vertices[(i + 3) % n], unit_circles_vertices[(i + 4) % n], z + 1);

        // Triangle with base on the lower circle
        vertex_positions.push_back(v1.x);
        vertex_positions.push_back(v1.y);
        vertex_positions.push_back(v1.z);

        vertex_positions.push_back(v2.x);
        vertex_positions.push_back(v2.y);
        vertex_positions.push_back(v2.z);

        vertex_positions.push_back(v4.x);
        vertex_positions.push_back(v4.y);
        vertex_positions.push_back(v4.z);

        // Triangle with base on the upper circle
        vertex_positions.push_back(v1.x);
        vertex_positions.push_back(v1.y);
        vertex_positions.push_back(v1.z);

        vertex_positions.push_back(v4.x);
        vertex_positions.push_back(v4.y);
        vertex_positions.push_back(v4.z);

        vertex_positions.push_back(v3.x);
        vertex_positions.push_back(v3.y);
        vertex_positions.push_back(v3.z);
    }

    // Add vertex positions for the triangles on the inside of the cylinder
    for (unsigned int i = 0; i < unit_circles_vertices.size(); i += 3) {

        glm::vec3 v1(unit_circles_vertices[i] * (1 - HollowCylinder::thickness),
                     unit_circles_vertices[i + 1] * (1 - HollowCylinder::thickness), z);
        glm::vec3 v2(unit_circles_vertices[(i + 3) % n] * (1 - HollowCylinder::thickness),
                     unit_circles_vertices[(i + 4) % n] * (1 - HollowCylinder::thickness), z);
        glm::vec3 v3(unit_circles_vertices[i] * (1 - HollowCylinder::thickness),
                     unit_circles_vertices[i + 1] * (1 - HollowCylinder::thickness), z + 1);
        glm::vec3 v4(unit_circles_vertices[(i + 3) % n] * (1 - HollowCylinder::thickness),
                     unit_circles_vertices[(i + 4) % n] * (1 - HollowCylinder::thickness), z + 1);

        // Triangle with base on the lower circle
        vertex_positions.push_back(v2.x);
        vertex_positions.push_back(v2.y);
        vertex_positions.push_back(v2.z);

        vertex_positions.push_back(v1.x);
        vertex_positions.push_back(v1.y);
        vertex_positions.push_back(v1.z);

        vertex_positions.push_back(v3.x);
        vertex_positions.push_back(v3.y);
        vertex_positions.push_back(v3.z);

        // Triangle with base on the upper circle
        vertex_positions.push_back(v3.x);
        vertex_positions.push_back(v3.y);
        vertex_positions.push_back(v3.z);

        vertex_positions.push_back(v4.x);
        vertex_positions.push_back(v4.y);
        vertex_positions.push_back(v4.z);

        vertex_positions.push_back(v2.x);
        vertex_positions.push_back(v2.y);
        vertex_positions.push_back(v2.z);
    }

    // Add triangles for the rings at the top and bottom of the cylinder
    for (unsigned int i = 0; i < unit_circles_vertices.size(); i += 3) {

        z = 0.5f;

        glm::vec3 v1(unit_circles_vertices[i], unit_circles_vertices[i + 1], z);
        glm::vec3 v2(unit_circles_vertices[(i + 3) % n], unit_circles_vertices[(i + 4) % n], z);
        glm::vec3 v3(unit_circles_vertices[i] * (1 - HollowCylinder::thickness),
                     unit_circles_vertices[i + 1] * (1 - HollowCylinder::thickness), z);
        glm::vec3 v4(unit_circles_vertices[(i + 3) % n] * (1 - HollowCylinder::thickness),
                     unit_circles_vertices[(i + 4) % n] * (1 - HollowCylinder::thickness), z);

        // Two triangles on the upper ring
        vertex_positions.push_back(v1.x);
        vertex_positions.push_back(v1.y);
        vertex_positions.push_back(v1.z);

        vertex_positions.push_back(v2.x);
        vertex_positions.push_back(v2.y);
        vertex_positions.push_back(v2.z);

        vertex_positions.push_back(v4.x);
        vertex_positions.push_back(v4.y);
        vertex_positions.push_back(v4.z);

        vertex_positions.push_back(v1.x);
        vertex_positions.push_back(v1.y);
        vertex_positions.push_back(v1.z);

        vertex_positions.push_back(v4.x);
        vertex_positions.push_back(v4.y);
        vertex_positions.push_back(v4.z);

        vertex_positions.push_back(v3.x);
        vertex_positions.push_back(v3.y);
        vertex_positions.push_back(v3.z);

        // Two triangles on the lower ring
        vertex_positions.push_back(v2.x);
        vertex_positions.push_back(v2.y);
        vertex_positions.push_back(v2.z - 1.0f);

        vertex_positions.push_back(v1.x);
        vertex_positions.push_back(v1.y);
        vertex_positions.push_back(v1.z - 1.0f);

        vertex_positions.push_back(v3.x);
        vertex_positions.push_back(v3.y);
        vertex_positions.push_back(v3.z - 1.0f);

        vertex_positions.push_back(v4.x);
        vertex_positions.push_back(v4.y);
        vertex_positions.push_back(v4.z - 1.0f);

        vertex_positions.push_back(v2.x);
        vertex_positions.push_back(v2.y);
        vertex_positions.push_back(v2.z - 1.0f);

        vertex_positions.push_back(v3.x);
        vertex_positions.push_back(v3.y);
        vertex_positions.push_back(v3.z - 1.0f);
    }

    return vertex_positions;
}

std::vector<float>
HollowCylinder::generateVertexNormals(const std::vector<float>& vertex_positions) {
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

void HollowCylinder::init() {
    // Get the vertices of the hollow cylinder from the circles on either end of the
    // cylinder

    std::vector<float> vertex_positions = HollowCylinder::generateVertexPositions();
    std::vector<float> vertex_normals   = generateVertexNormals(vertex_positions);

    glGenBuffers(1, &HollowCylinder::VBO);
    glGenVertexArrays(1, &HollowCylinder::VAO);

    glBindVertexArray(HollowCylinder::VAO);

    glBindBuffer(GL_ARRAY_BUFFER, HollowCylinder::VBO);
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

std::shared_ptr<HollowCylinder> createHollowCylinderFromData(std::string& data) {
    std::stringstream ss(data);
    std::string str;

    std::shared_ptr<HollowCylinder> temp =
        std::make_shared<HollowCylinder>(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0),
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
