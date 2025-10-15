#include "renderer.hpp"

Renderer::Renderer(int window_width, int window_height, const unsigned int max_lights)
    : window_width(window_width)
    , window_height(window_height), max_lights(max_lights) {
    // Initialise other variables
    num_lights = 0;
    subsamples = 4;
}

Renderer::~Renderer() {
    glDeleteFramebuffers(1, &multisample_fbo);
    glDeleteFramebuffers(1, &intermediate_fbo);
}

void Renderer::init() {
    initShaders();
    initFramebuffer();
    initSkyboxes();
    initScreenQuad();

    configUniformBufferObjects();

    // Set OpenGL flags
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
}

void Renderer::initShaders() {
    shader_lib.create("blinn_phong", RESOURCES_PATH "vshader.glsl", RESOURCES_PATH "fshader.glsl",
                      "");
    shader_lib.create("outline", RESOURCES_PATH "outline_vshader.glsl",
                      RESOURCES_PATH "outline_fshader.glsl", "");
    shader_lib.create("lights", RESOURCES_PATH "vshader.glsl", RESOURCES_PATH "light_fshader.glsl",
                      "");
    shader_lib.create("skybox", RESOURCES_PATH "skybox_vshader.glsl",
                      RESOURCES_PATH "skybox_fshader.glsl", "");
    shader_lib.create("normals", RESOURCES_PATH "normals_vshader.glsl",
                      RESOURCES_PATH "normals_fshader.glsl", RESOURCES_PATH "normals_gshader.glsl");
    shader_lib.create("screen", RESOURCES_PATH "screen_vshader.glsl",
                      RESOURCES_PATH "screen_fshader.glsl", "");
}

void Renderer::initSkyboxes() {
    Skybox::init();
    std::vector<std::string> skybox_faces = {RESOURCES_PATH "textures/brightsky/right.jpg",
                                             RESOURCES_PATH "textures/brightsky/left.jpg",
                                             RESOURCES_PATH "textures/brightsky/top.jpg",
                                             RESOURCES_PATH "textures/brightsky/bottom.jpg",
                                             RESOURCES_PATH "textures/brightsky/front.jpg",
                                             RESOURCES_PATH "textures/brightsky/back.jpg"};

    unsigned int brightsky_texture = TextureUtility::loadCubeMapTexture(skybox_faces);

    skybox_faces = {RESOURCES_PATH "textures/starrysky/right.jpg",
                    RESOURCES_PATH "textures/starrysky/left.jpg",
                    RESOURCES_PATH "textures/starrysky/top.jpg",
                    RESOURCES_PATH "textures/starrysky/bottom.jpg",
                    RESOURCES_PATH "textures/starrysky/front.jpg",
                    RESOURCES_PATH "textures/starrysky/back.jpg"};

    unsigned int starrysky_texture = TextureUtility::loadCubeMapTexture(skybox_faces);

    skybox_texture_map["brightsky"] = brightsky_texture;
    skybox_texture_map["starrysky"] = starrysky_texture;
    active_skybox_texture_name      = "brightsky";
}

void Renderer::initFramebuffer() {
    glGenFramebuffers(1, &multisample_fbo);
    glGenTextures(1, &multisample_texture);
    glGenRenderbuffers(1, &multisample_rbo);

    // Colour attachment
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisample_texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, subsamples, GL_RGB, window_width,
                            window_height, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Stencil and buffer attachment
    glBindRenderbuffer(GL_RENDERBUFFER, multisample_rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, subsamples, GL_DEPTH24_STENCIL8, window_width,
                                     window_height);

    glBindFramebuffer(GL_FRAMEBUFFER, multisample_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE,
                           multisample_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                              multisample_rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Multisample Framebuffer is not complete!" << std::endl;

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Now for the intermediate framebuffer and the screen quad texture
    glGenFramebuffers(1, &intermediate_fbo);
    glGenTextures(1, &screen_texture);

    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, intermediate_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Intermediate Framebuffer is not complete!" << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::initScreenQuad() {
    float vertices[] = {-1.0f, 1.0f,  0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
                        1.0f,  -1.0f, 1.0f, 0.0f, -1.0f, 1.0f,  0.0f, 1.0f,
                        1.0f,  -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  1.0f, 1.0f};

    glGenBuffers(1, &screen_vbo);
    glGenVertexArrays(1, &screen_vao);

    glBindVertexArray(screen_vao);
    glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                            (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::configUniformBufferObjects() {
    // Create the UBO for view and projection matrices
    glGenBuffers(1, &matrices_ubo);

    // Allocate the memory for it
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

    // Bind to binding point 0
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, matrices_ubo, 0, 2 * sizeof(glm::mat4));

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Now the shaders
    shader_lib.get("blinn_phong").setUniformBlockBinding("Matrices", 0);
    shader_lib.get("outline").setUniformBlockBinding("Matrices", 0);
    shader_lib.get("lights").setUniformBlockBinding("Matrices", 0);
    shader_lib.get("normals").setUniformBlockBinding("Matrices", 0);
}

void Renderer::setLightUniforms(Shader& shader, std::vector<std::shared_ptr<GameObject>>& objects) {
    shader.use();
    unsigned int counter = 0;

    for (unsigned int i = 0; i < objects.size(); ++i) {

        if (counter > max_lights) {
            std::cout << "Counter somehow greater than number of max light" << std::endl;
        }

        if (objects[i]->light) {
            shader.setVec3("point_lights[" + std::to_string(counter) + "].position",
                           objects[i]->pos);
            shader.setVec3("point_lights[" + std::to_string(counter) + "].colour",
                           objects[i]->colour);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].ambient",
                            objects[i]->light->ambient);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].diffuse",
                            objects[i]->light->diffuse);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].specular",
                            objects[i]->light->specular);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].constant",
                            objects[i]->light->constant);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].linear",
                            objects[i]->light->linear);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].quadratic",
                            objects[i]->light->quadratic);

            counter++;
        }
    }
}

void Renderer::processScreenResize(int new_window_width, int new_window_height) {
    window_width = new_window_width;
    window_height = new_window_height;

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisample_texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, subsamples, GL_RGB, window_width, window_height,
                            GL_TRUE);
    glBindRenderbuffer(GL_RENDERBUFFER, multisample_rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, subsamples, GL_DEPTH24_STENCIL8, window_width,
                                     window_height);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Now for the intermediate framebuffer and the screen quad texture
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::renderPrep(Camera* camera) {
    // Render to multisample framebuffer to write to multisample texture
    glBindFramebuffer(GL_FRAMEBUFFER, multisample_fbo);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    configUniformBufferObjects();
    // Consider throwing the below into the function above
    glm::mat4 view       = camera->lookAt();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera->fov),
        (static_cast<float>(window_width) / static_cast<float>(window_height)), 0.1f, 100.f);

    // Fill the UBO with view and projection matrices
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Renderer::renderScene(std::vector<std::shared_ptr<GameObject>>& objects, Camera* camera) {
    // View and projection matrices won't change between objects
    glm::mat4 view       = camera->lookAt();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera->fov),
        (static_cast<float>(window_width) / static_cast<float>(window_height)), 0.1f, 100.f);

    // Fill the UBO with view and projection matrices
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shader_lib.get("blinn_phong").use();
    setLightUniforms(shader_lib.get("blinn_phong"), objects);
    shader_lib.get("blinn_phong").setInt("point_lights_number", num_lights);
    shader_lib.get("blinn_phong").setVec3("viewer_pos", camera->pos);

    shader_lib.get("skybox").use();
    // Keeping the upper 3x3 of the view matrix removes the element of translation from it.
    // Skybox will stay centred around the camera
    shader_lib.get("skybox").setMat("view", glm::mat4(glm::mat3(view)));
    shader_lib.get("skybox").setMat("projection", projection);

    Skybox::draw(shader_lib.get("skybox"), skybox_texture_map[active_skybox_texture_name]);

    // Draw objects that don't need to be outlined first first

    for (int i = 0; i < objects.size(); ++i) {
        // Draw lights with one shader, objects in another
        if (objects[i]->light) {
            objects[i]->draw(shader_lib.get("lights"));
        } else {
            objects[i]->draw(shader_lib.get("blinn_phong"));
        }
    }

    // Draw objects that need to be outlined
    shader_lib.get("outline").use();
    shader_lib.get("outline").setMat("view", view);
    shader_lib.get("outline").setMat("projection", projection);
}

void Renderer::renderOutlinedObjects(std::shared_ptr<GameObject> selected_object,
                                     std::shared_ptr<GameObject> mouseover_object) {
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // Draw selected object
    if (selected_object) {

        if (selected_object->light) {
            selected_object->draw(shader_lib.get("lights"));
        } else {
            selected_object->draw(shader_lib.get("blinn_phong"));
        }
        // Draw outline of selected object
        selected_object->scale *= 1.05;
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);

        glDisable(GL_DEPTH_TEST);

        selected_object->draw(shader_lib.get("outline"));

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
        glStencilMask(0xFF);
        selected_object->scale /= 1.05;
    }

    // Draw mouseover object
    if (mouseover_object) {
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        if (mouseover_object->light) {
            mouseover_object->draw(shader_lib.get("lights"));
        } else {
            mouseover_object->draw(shader_lib.get("blinn_phong"));
        }

        // Draw outline of mouseover object
        mouseover_object->scale *= 1.05;
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);

        glDisable(GL_DEPTH_TEST);

        mouseover_object->draw(shader_lib.get("outline"));

        // Reset mouseover object size and stencil, depth buffer settings
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
        glStencilMask(0xFF);
        mouseover_object->scale /= 1.05;
    }

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void Renderer::renderBbox(std::shared_ptr<GameObject> object, Cube& bbox_wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    bbox_wireframe.pos    = object->pos;
    bbox_wireframe.colour = object->colour;

    bbox_wireframe.scale.x = object->bbox.xmax - object->bbox.xmin;
    bbox_wireframe.scale.y = object->bbox.ymax - object->bbox.ymin;
    bbox_wireframe.scale.z = object->bbox.zmax - object->bbox.zmin;

    // Going to use a simple fragment shader for the wireframe box
    bbox_wireframe.draw(shader_lib.get("lights"));

    // Set polygon mode back to normal after rendering the wireframe box
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::renderNormals(std::vector<std::shared_ptr<GameObject>>& objects) {
    // Render normals
    for (int i = 0; i < objects.size(); ++i) {
        objects[i]->draw(shader_lib.get("normals"));
    }
}

void Renderer::renderGizmos(std::unordered_map<std::string, std::shared_ptr<Gizmo>>& gizmos,
                            std::shared_ptr<Gizmo> mouseover_gizmo, GizmoType active_gizmo_type) {
    glDisable(GL_DEPTH_TEST);

    float scaling_factor = 1.0f;
    if (active_gizmo_type == GizmoType::MOVE) {
        // If move gizmos are active:
        scaling_factor = 1.2f;
    } else if (active_gizmo_type == GizmoType::ROTATE) {
        scaling_factor = 1.05f;

        // Enable depth testing for rotation gizmos
        glEnable(GL_DEPTH_TEST);
    }

    // Make gizmo mouse is currently over invisble until it is drawn larger
    if (mouseover_gizmo) {
        mouseover_gizmo->body->visible = false;
    }

    for (auto& it : gizmos) {
        if (it.second->getActivity()) {
            it.second->draw(shader_lib.get("lights"));
        }
    }

    // Render the gizmo mouse is currently over but larger and brighter
    if (mouseover_gizmo) {
        mouseover_gizmo->body->visible = true;
        mouseover_gizmo->body->scale *= scaling_factor;
        mouseover_gizmo->body->colour *= 1.2f;
        mouseover_gizmo->draw(shader_lib.get("lights"));
        mouseover_gizmo->body->scale /= scaling_factor;
        mouseover_gizmo->body->colour /= 1.2f;
    }

    // Disablle depth testing for the centre gizmo
    glDisable(GL_DEPTH_TEST);

    // Want the centre gizmo to always be visible so draw it last
    //centre_gizmo->draw(shader_lib.get("lights"));

    glEnable(GL_DEPTH_TEST);
}

void Renderer::renderScreen() {
    // Blit multisample framebuffer to intermediate framebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, multisample_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediate_fbo);
    glBlitFramebuffer(0, 0, window_width, window_height, 0, 0, window_width, window_height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(screen_vao);
    shader_lib.get("screen").use();
    shader_lib.get("screen").setInt("screenTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

std::string& Renderer::get_active_skybox_name() {
    return active_skybox_texture_name;
}

std::unordered_map<std::string, unsigned int>& Renderer::get_skyboxes() {
    return skybox_texture_map;
}

void Renderer::setNumLights(unsigned int num_lights) {
    this->num_lights = num_lights;
}
