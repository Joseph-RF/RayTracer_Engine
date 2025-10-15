#include <cube.hpp>

unsigned int Cube::VBO;
unsigned int Cube::EBO;
unsigned int Cube::VAO;

Cube::Cube() {
    this->pos         = glm::vec3(0.0, 0.0, 0.0);
    this->orientation = glm::vec3(0.0, 0.0, 0.0);
    this->scale       = glm::vec3(1.0, 1.0, 1.0);
    this->colour      = glm::vec3(1.0, 0.0, 0.0);
    this->shininess   = 32.0f;

    update_bounding_box();
    this->name    = "NO_NAME";
    this->light   = nullptr;
    this->visible = true;
}

Cube::Cube(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour,
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

void Cube::draw(Shader& shader) {
    if (!visible) {
        return;
    }

    glBindVertexArray(Cube::VAO);
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

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Cube::update_bounding_box() {
    // Update the bounding box
    std::vector<glm::vec3> vert = {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),
                                   glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(0.5f, -0.5f, 0.5f),
                                   glm::vec3(-0.5f, 0.5f, -0.5f),  glm::vec3(0.5f, 0.5f, -0.5f),
                                   glm::vec3(-0.5f, 0.5f, 0.5f),   glm::vec3(0.5f, 0.5f, 0.5f)};

    for (unsigned int i = 0; i < vert.size(); ++i) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);
        model = glm::rotate(model, orientation.x, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, orientation.y, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, orientation.z, glm::vec3(0.0, 0.0, 1.0));
        model = glm::scale(model, scale);

        vert[i] = glm::vec3(model * glm::vec4(vert[i], 1.0));
    }

    // Default bbox given the cube's vertices
    // Adjust depending on the transformations
    // NOTE: Assume that it won't change later
    bbox = AABB(vert[0].x, vert[0].x, vert[0].y, vert[0].y, vert[0].z, vert[0].z);

    for (unsigned int i = 1; i < vert.size(); ++i) {
        if (vert[i].x < bbox.xmin) {
            bbox.xmin = vert[i].x;
        }
        if (vert[i].x > bbox.xmax) {
            bbox.xmax = vert[i].x;
        }
        if (vert[i].y < bbox.ymin) {
            bbox.ymin = vert[i].y;
        }
        if (vert[i].y > bbox.ymax) {
            bbox.ymax = vert[i].y;
        }
        if (vert[i].z < bbox.zmin) {
            bbox.zmin = vert[i].z;
        }
        if (vert[i].z > bbox.zmax) {
            bbox.zmax = vert[i].z;
        }
    }
}

void Cube::add_light(float ambient, float diffuse, float specular, float constant, float linear,
                     float quadratic) {
    this->light = std::make_unique<Light>(ambient, diffuse, specular, constant, linear, quadratic);
}

std::string Cube::dataToString() {
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

void Cube::init() {
    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f};

    glGenBuffers(1, &Cube::VBO);
    glGenVertexArrays(1, &Cube::VAO);

    glBindVertexArray(Cube::VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Cube::VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

std::shared_ptr<Cube> createCubeFromData(std::string& data) {
    std::stringstream ss(data);
    std::string str;

    std::shared_ptr<Cube> temp =
        std::make_shared<Cube>(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0),
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
