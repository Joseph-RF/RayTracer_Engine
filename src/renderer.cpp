#include "renderer.hpp"

Renderer::Renderer(int window_width, int window_height, const unsigned int max_lights)
    : window_width(window_width)
    , window_height(window_height)
    , max_lights(max_lights) {
    // Initialise other variables
    shadow_width             = 1024;
    shadow_height            = 1024;
    depth_map_fbo            = 0;
    depth_map_texture_offset = 2; // Depth map texture will start at GL_TEXTURE2

    num_lights = 0;
    subsamples = 4;

    matrices_ubo     = 0;
    multisample_fbo  = 0;
    multisample_rbo  = 0;
    intermediate_fbo = 0;

    screen_vbo = 0;
    screen_vao = 0;

    multisample_texture = 0;
    screen_texture      = 0;

    draw_normals = false;
    use_pcf      = false;
}

Renderer::~Renderer() {
    glDeleteFramebuffers(1, &multisample_fbo);
    glDeleteFramebuffers(1, &intermediate_fbo);
}

void Renderer::init() {
    initShaders();
    initFramebuffers();
    initSkyboxes();
    initScreenQuad();

    // Set OpenGL flags
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    // SHADOWS
    // ----------------
    shader_lib.get("blinn_phong").use();
    shader_lib.get("blinn_phong").setInt("depth_map", 2);
}

void Renderer::render(const RenderContext& render_context) {
    renderPrep(render_context.objects, render_context.camera);

    // Render scene
    renderScene(render_context);

    // Render normals
    if (draw_normals) {
        renderNormals(render_context.objects);
    }

    // Render gizmos
    if (render_context.selected_object) {
        renderGizmos(render_context.gizmos, render_context.mouseover_gizmo,
                     render_context.active_gizmo_type);
    }

    // Render to screen
    renderScreen();
}

void Renderer::initShaders() {
    shader_lib.create("blinn_phong", SHADERS_PATH "vshader.glsl", SHADERS_PATH "fshader.glsl", "");
    shader_lib.create("outline", SHADERS_PATH "outline_vshader.glsl",
                      SHADERS_PATH "outline_fshader.glsl", "");
    shader_lib.create("lights", SHADERS_PATH "vshader.glsl", SHADERS_PATH "light_fshader.glsl", "");
    shader_lib.create("skybox", SHADERS_PATH "skybox_vshader.glsl",
                      SHADERS_PATH "skybox_fshader.glsl", "");
    shader_lib.create("normals", SHADERS_PATH "normals_vshader.glsl",
                      SHADERS_PATH "normals_fshader.glsl", SHADERS_PATH "normals_gshader.glsl");
    shader_lib.create("screen", SHADERS_PATH "screen_vshader.glsl",
                      SHADERS_PATH "screen_fshader.glsl", "");
    shader_lib.create("shadows", SHADERS_PATH "point_shadows_vshader.glsl",
                      SHADERS_PATH "point_shadows_fshader.glsl",
                      SHADERS_PATH "point_shadows_gshader.glsl");
}

void Renderer::initSkyboxes() {
    Skybox::init();
    std::vector<std::string> skybox_faces = {
        TEXTURES_PATH "brightsky/right.jpg", TEXTURES_PATH "brightsky/left.jpg",
        TEXTURES_PATH "brightsky/top.jpg",   TEXTURES_PATH "brightsky/bottom.jpg",
        TEXTURES_PATH "brightsky/front.jpg", TEXTURES_PATH "brightsky/back.jpg"};

    unsigned int brightsky_texture = TextureUtility::loadCubeMapTexture(skybox_faces);

    skybox_faces = {TEXTURES_PATH "starrysky/right.jpg", TEXTURES_PATH "starrysky/left.jpg",
                    TEXTURES_PATH "starrysky/top.jpg",   TEXTURES_PATH "starrysky/bottom.jpg",
                    TEXTURES_PATH "starrysky/front.jpg", TEXTURES_PATH "starrysky/back.jpg"};

    unsigned int starrysky_texture = TextureUtility::loadCubeMapTexture(skybox_faces);

    skybox_texture_map["brightsky"] = brightsky_texture;
    skybox_texture_map["starrysky"] = starrysky_texture;
    active_skybox_texture_name      = "brightsky";
}

void Renderer::initFramebuffers() {
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

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Multisample Framebuffer is not complete!" << std::endl;
    }

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

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Intermediate Framebuffer is not complete!" << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Depth map framebuffer object for shadows
    glGenFramebuffers(1, &depth_map_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::initScreenQuad() {
    float vertices[] = {-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
                        -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

    glGenBuffers(1, &screen_vbo);
    glGenVertexArrays(1, &screen_vao);

    glBindVertexArray(screen_vao);
    glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::setUniformBufferObjects() {
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

void Renderer::setLightUniforms(Shader& shader,
                                std::vector<std::shared_ptr<GameObject>>& objects) const {
    shader.use();
    unsigned int counter = 0;

    // Empty the shader of the point lights
    for (unsigned int i = 0; i < max_lights; ++i) {
        shader.setVec3("point_lights[" + std::to_string(counter) + "].position", glm::vec3(0.0));
        shader.setVec3("point_lights[" + std::to_string(counter) + "].colour", glm::vec3(0.0));
        shader.setFloat("point_lights[" + std::to_string(counter) + "].ambient", 0.0);
        shader.setFloat("point_lights[" + std::to_string(counter) + "].diffuse", 0.0);
        shader.setFloat("point_lights[" + std::to_string(counter) + "].specular", 0.0);
        shader.setFloat("point_lights[" + std::to_string(counter) + "].constant", 0.0);
        shader.setFloat("point_lights[" + std::to_string(counter) + "].linear", 0.0);
        shader.setFloat("point_lights[" + std::to_string(counter) + "].quadratic", 0.0);
    }

    for (const std::shared_ptr<GameObject>& object : objects) {
        if (counter > max_lights) {
            std::cout << "Counter somehow greater than number of max light" << std::endl;
        }

        if (object->light) {
            shader.setVec3("point_lights[" + std::to_string(counter) + "].position", object->pos);
            shader.setVec3("point_lights[" + std::to_string(counter) + "].colour", object->colour);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].ambient",
                            object->light->ambient);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].diffuse",
                            object->light->diffuse);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].specular",
                            object->light->specular);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].constant",
                            object->light->constant);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].linear",
                            object->light->linear);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].quadratic",
                            object->light->quadratic);

            // Set uniforms for depth map
            shader.setInt("depth_maps[" + std::to_string(counter) + "]",
                          depth_map_texture_offset + counter);

            counter++;
        }
    }
}

void Renderer::createDepthMap(std::vector<std::shared_ptr<GameObject>>& objects) {
    constexpr float near_plane = 1.0f;
    constexpr float far_plane  = 25.0f;

    shader_lib.get("blinn_phong").use();
    shader_lib.get("blinn_phong").setFloat("far_plane", far_plane);

    std::vector<glm::mat4> shadow_transforms(6);
    glm::mat4 shadow_projection = glm::perspective(
        glm::radians(90.0f), static_cast<float>(shadow_width) / static_cast<float>(shadow_height),
        near_plane, far_plane);

    // Cull front faces instead of back faces to help prevent peter panning
    glCullFace(GL_FRONT);

    for (std::shared_ptr<GameObject>& object : objects) {
        if (!object->light) {
            continue;
        }
        // Create depth map textures for lights if there are any that haven't been made yet
        object->light->createDepthMapTexture(shadow_width, shadow_height);

        glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, object->light->depth_map, 0);

        glClear(GL_DEPTH_BUFFER_BIT);

        // Do the shadow pass
        glViewport(0, 0, shadow_width, shadow_height);

        // Fill in the shadow transforms
        shadow_transforms[0] =
            shadow_projection * glm::lookAt(object->pos, object->pos + glm::vec3(1.0f, 0.0f, 0.0f),
                                            glm::vec3(0.0f, -1.0f, 0.0f));
        shadow_transforms[1] =
            shadow_projection * glm::lookAt(object->pos, object->pos + glm::vec3(-1.0f, 0.0f, 0.0f),
                                            glm::vec3(0.0f, -1.0f, 0.0f));
        shadow_transforms[2] =
            shadow_projection * glm::lookAt(object->pos, object->pos + glm::vec3(0.0f, 1.0f, 0.0f),
                                            glm::vec3(0.0f, 0.0f, 1.0f));
        shadow_transforms[3] =
            shadow_projection * glm::lookAt(object->pos, object->pos + glm::vec3(0.0f, -1.0f, 0.0f),
                                            glm::vec3(0.0f, 0.0f, -1.0f));
        shadow_transforms[4] =
            shadow_projection * glm::lookAt(object->pos, object->pos + glm::vec3(0.0f, 0.0f, 1.0f),
                                            glm::vec3(0.0f, -1.0f, 0.0f));
        shadow_transforms[5] =
            shadow_projection * glm::lookAt(object->pos, object->pos + glm::vec3(0.0f, 0.0f, -1.0f),
                                            glm::vec3(0.0f, -1.0f, 0.0f));

        shader_lib.get("shadows").use();
        for (unsigned int i = 0; i < 6; ++i) {
            shader_lib.get("shadows").setMat("shadow_matrices[" + std::to_string(i) + "]",
                                             shadow_transforms[i]);
        }
        shader_lib.get("shadows").setFloat("far_plane", far_plane);
        shader_lib.get("shadows").setVec3("light_pos", objects[2]->pos);

        for (std::shared_ptr<GameObject>& render_target : objects) {
            if (render_target == object) {
                continue;
            }
            render_target->draw(shader_lib.get("shadows"));
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Go back to culling back faces
    glCullFace(GL_BACK);

    glViewport(0, 0, window_width, window_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::processScreenResize(int new_window_width, int new_window_height) {
    window_width  = new_window_width;
    window_height = new_window_height;

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisample_texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, subsamples, GL_RGB, window_width,
                            window_height, GL_TRUE);
    glBindRenderbuffer(GL_RENDERBUFFER, multisample_rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, subsamples, GL_DEPTH24_STENCIL8, window_width,
                                     window_height);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Now for the intermediate framebuffer and the screen quad texture
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::renderPrep(std::vector<std::shared_ptr<GameObject>>& objects, Camera* camera) {
    // Render to multisample framebuffer to write to multisample texture
    glBindFramebuffer(GL_FRAMEBUFFER, multisample_fbo);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    setUniformBufferObjects();
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

void Renderer::renderScene(const RenderContext& render_context) {
    // View and projection matrices won't change between objects
    glm::mat4 view       = render_context.camera->lookAt();
    glm::mat4 projection = glm::perspective(
        glm::radians(render_context.camera->fov),
        (static_cast<float>(window_width) / static_cast<float>(window_height)), 0.1f, 100.f);

    // Fill the UBO with view and projection matrices
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shader_lib.get("skybox").use();
    // Keeping the upper 3x3 of the view matrix removes the element of translation from it.
    // Skybox will stay centred around the camera
    shader_lib.get("skybox").setMat("view", glm::mat4(glm::mat3(view)));
    shader_lib.get("skybox").setMat("projection", projection);
    Skybox::draw(shader_lib.get("skybox"), skybox_texture_map[active_skybox_texture_name]);

    // Create the shadow maps for the light sources
    createDepthMap(render_context.objects);

    shader_lib.get("blinn_phong").use();
    setLightUniforms(shader_lib.get("blinn_phong"), render_context.objects);
    shader_lib.get("blinn_phong").setInt("point_lights_number", num_lights);
    shader_lib.get("blinn_phong").setVec3("viewer_pos", render_context.camera->pos);
    shader_lib.get("blinn_phong").setBool("use_pcf", use_pcf);

    // Bind the depth map textures
    unsigned int light_counter = 0;
    for (const std::shared_ptr<GameObject>& object : render_context.objects) {
        if (!object->light) {
            continue;
        }
        glActiveTexture(GL_TEXTURE0 + depth_map_texture_offset + light_counter);
        glBindTexture(GL_TEXTURE_CUBE_MAP, object->light->depth_map);

        light_counter++;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, multisample_fbo);

    // Save rendering of selected and mouseover objects for later
    if (render_context.mouseover_object) {
        render_context.mouseover_object->visible = false;
    }
    if (render_context.selected_object) {
        render_context.selected_object->visible = false;
    }

    for (const std::shared_ptr<GameObject>& object : render_context.objects) {
        // Draw lights with one shader, objects in another
        if (object->light) {
            object->draw(shader_lib.get("lights"));
        } else {
            object->draw(shader_lib.get("blinn_phong"));
        }
    }
    // Render outlined and selected object
    if (render_context.mouseover_object) {
        render_context.mouseover_object->visible = true;
        renderOutlinedObject(*render_context.mouseover_object);
        renderBbox(*render_context.mouseover_object);
    }
    if (render_context.selected_object) {
        render_context.selected_object->visible = true;
        renderOutlinedObject(*render_context.selected_object);
        renderBbox(*render_context.selected_object);
    }
}

void Renderer::renderOutlinedObject(GameObject& object) {
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    if (object.light) {
        object.draw(shader_lib.get("lights"));
    } else {
        object.draw(shader_lib.get("blinn_phong"));
    }
    // Draw outline of selected object
    object.scale *= 1.05;
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);

    glDisable(GL_DEPTH_TEST);

    object.draw(shader_lib.get("outline"));

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);
    glStencilMask(0xFF);
    object.scale /= 1.05;

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void Renderer::renderBbox(const GameObject& object) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    bbox_wireframe.pos    = object.pos;
    bbox_wireframe.colour = object.colour;

    bbox_wireframe.scale.x = object.bbox.xmax - object.bbox.xmin;
    bbox_wireframe.scale.y = object.bbox.ymax - object.bbox.ymin;
    bbox_wireframe.scale.z = object.bbox.zmax - object.bbox.zmin;

    // Going to use a simple fragment shader for the wireframe box
    bbox_wireframe.draw(shader_lib.get("lights"));

    // Set polygon mode back to normal after rendering the wireframe box
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::renderNormals(std::vector<std::shared_ptr<GameObject>>& objects) {
    // Render normals
    for (const std::shared_ptr<GameObject>& object : objects) {
        object->draw(shader_lib.get("normals"));
    }
}

void Renderer::renderGizmos(std::unordered_map<std::string, std::shared_ptr<Gizmo>>& gizmos,
                            const std::shared_ptr<Gizmo>& mouseover_gizmo,
                            GizmoType active_gizmo_type) {
    glDisable(GL_DEPTH_TEST);

    float scaling_factor;
    if (active_gizmo_type == GizmoType::ROTATE) {
        scaling_factor = 1.05f;

        // Enable depth testing for rotation gizmos
        glEnable(GL_DEPTH_TEST);
    } else {
        scaling_factor = 1.2f;
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
    // centre_gizmo->draw(shader_lib.get("lights"));

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
