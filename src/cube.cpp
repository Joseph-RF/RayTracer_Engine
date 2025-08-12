#include <cube.hpp>

unsigned int Cube::VBO;
unsigned int Cube::EBO;
unsigned int Cube::VAO;

Cube::Cube(
    glm::vec3 pos,
    glm::vec3 orientation,
    float size,
    glm::vec3 colour,
    float shininess
) {
	this->pos = pos;
	this->orientation = orientation;
	this->size = size;
	this->colour = colour;
    this->shininess = shininess;

    update_bounding_box();
    name = "NO_NAME";
    light = nullptr;
}

void Cube::draw(Shader& shader) {
    glBindVertexArray(Cube::VAO);
    shader.use();

    glm::mat4 model(1.0);
    model = glm::translate(model, pos);
    model = glm::rotate(model, orientation.x, glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, orientation.y, glm::vec3(0.0, 1.0, 0.0));
    model = glm::rotate(model, orientation.z, glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, size * glm::vec3(1.0, 1.0, 1.0));
    shader.setMat("model", model);
    shader.setVec3("colour", colour);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

AABB Cube::update_bounding_box() {
    // Update the bounding box
    std::vector<glm::vec3> vert = {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f,  0.5f),
        glm::vec3(0.5f, -0.5f,  0.5f),
        glm::vec3(-0.5f,  0.5f, -0.5f),
        glm::vec3(0.5f,  0.5f, -0.5f),
        glm::vec3(-0.5f,  0.5f,  0.5f),
        glm::vec3(0.5f,  0.5f,  0.5f)
    };

    for (unsigned int i = 0; i < vert.size(); ++i) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);
        model = glm::rotate(model, orientation.x, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, orientation.y, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, orientation.z, glm::vec3(0.0, 0.0, 1.0));
        model = glm::scale(model, size * glm::vec3(1.0, 1.0, 1.0));

        vert[i] = glm::vec3(model * glm::vec4(vert[i], 1.0));
    }

    // Default bbox given the cube's vertices
    // Adjust depending on the transformations
    // NOTE: Assume that it won't change later
    bbox = AABB(vert[0].x, vert[0].x, vert[0].y, vert[0].y, vert[0].z, vert[0].z);

    for (unsigned int i = 0; i < vert.size(); ++i) {
        if (vert[i].x < bbox.xmin) { bbox.xmin = vert[i].x; }
        if (vert[i].x > bbox.xmax) { bbox.xmax = vert[i].x; }
        if (vert[i].y < bbox.ymin) { bbox.ymin = vert[i].y; }
        if (vert[i].y > bbox.ymax) { bbox.ymax = vert[i].y; }
        if (vert[i].z < bbox.zmin) { bbox.zmin = vert[i].z; }
        if (vert[i].z > bbox.zmax) { bbox.zmax = vert[i].z; }
    }
    return bbox;
}

void Cube::add_light(
    float ambient,
    float diffuse,
    float specular,
    float constant,
    float linear,
    float quadratic
) {
    this->light = std::make_unique<Light>(
        ambient, diffuse, specular, constant, linear, quadratic
    );
}


void Cube::init() {
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glGenBuffers(1, &Cube::VBO);
    glGenVertexArrays(1, &Cube::VAO);

    glBindVertexArray(Cube::VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Cube::VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Vertex texture coords
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}