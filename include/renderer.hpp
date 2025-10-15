#pragma once

#include <camera.hpp>
#include <cube.hpp>
#include <gizmo.hpp>
#include <shader.hpp>
#include <skybox.hpp>
#include <texture_utility.hpp>

class Renderer {
public:
    Renderer(int window_width, int window_height, const unsigned int max_lights);
    ~Renderer();

    void init();

    void renderPrep(Camera* camera);
    void renderScene(std::vector<std::shared_ptr<GameObject>>& objects, Camera* camera);
    void renderOutlinedObjects(std::shared_ptr<GameObject> selected_object,
                               std::shared_ptr<GameObject> mouseover_object);
    void renderBbox(std::shared_ptr<GameObject> object, Cube& bbox_wireframe);
    void renderNormals(std::vector<std::shared_ptr<GameObject>>& objects);
    void renderGizmos(std::unordered_map<std::string, std::shared_ptr<Gizmo>>& gizmos,
                      std::shared_ptr<Gizmo> mouseover_gizmo, GizmoType active_gizmo_type);
    void renderScreen();

    void processScreenResize(int new_window_width, int new_window_height);

    std::string& get_active_skybox_name();
    std::unordered_map<std::string, unsigned int>& get_skyboxes();

    void setNumLights(unsigned int num_lights);

private:
    void initShaders();
    void initSkyboxes();
    void initFramebuffer();
    void initScreenQuad();

    void configUniformBufferObjects();

    void setLightUniforms(Shader& shader, std::vector<std::shared_ptr<GameObject>>& objects);

    // Window properties
    int window_width;
    int window_height;

    // Shaders
    ShaderLibrary shader_lib;

    unsigned int num_lights;
    const unsigned int max_lights;

    // Subsamples
    unsigned int subsamples;

    // Buffer objects
    unsigned int matrices_ubo;
    unsigned int multisample_fbo;
    unsigned int multisample_rbo;
    unsigned int intermediate_fbo;

    // Screen quad buffers
    unsigned int screen_vbo;
    unsigned int screen_vao;

    // Textures
    unsigned int multisample_texture;
    unsigned int screen_texture;

    // Skybox
    // Texture image file names for skybox faces
    std::string active_skybox_texture_name;
    std::unordered_map<std::string, unsigned int> skybox_texture_map;
};
