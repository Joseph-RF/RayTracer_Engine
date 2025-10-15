#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>
#include <typeinfo>

#include <arrow.hpp>
#include <camera.hpp>
#include <cube.hpp>
#include <gizmo.hpp>
#include <glfwwindowmanager.hpp>
#include <hollow_cylinder.hpp>
#include <math.hpp>
#include <scenesaver.hpp>
#include <shader.hpp>
#include <skybox.hpp>
#include <sphere.hpp>
#include <texture_utility.hpp>
#include <renderer.hpp>

class App {
public:
    App(int window_x, int window_y);
    ~App();

    void run();

    std::vector<std::shared_ptr<GameObject>> game_objects;
    unsigned int num_lights;
    void resetObjectPointers();

private:
    // Window properties
    int window_x;
    int window_y;
    std::unique_ptr<IWindowManager> window_manager;
    std::shared_ptr<EventManager> event_manager;

    // Timings
    float delta_time;
    float last_frame;

    // Mouse
    bool first_mouse_movement;
    float last_mouse_xpos;
    float last_mouse_ypos;
    bool mouse_pressed;

    // Camera
    Camera* active_camera;
    Camera engine_camera;
    Camera raytracer_camera;

    // Renderer
    Renderer renderer;

    // unsigned int num_lights;
    const unsigned int max_lights;

    // Objects
    std::shared_ptr<GameObject> mouseover_object;
    std::shared_ptr<GameObject> selected_object;

    // Data for placeholder object
    glm::vec3 placeholder_pos;
    glm::vec3 placeholder_orientation;
    glm::vec3 placeholder_scale;
    glm::vec3 placeholder_colour;
    float placeholder_shininess;
    Light placeholder_light;

    std::string placeholder_object_type;
    std::vector<std::string> object_type_list;

    // Gizmos
    std::shared_ptr<Gizmo> mouseover_gizmo;
    std::shared_ptr<Sphere> centre_gizmo;

    std::unordered_map<std::string, std::shared_ptr<Gizmo>> gizmos;

    // Gizmo function utility
    bool using_gizmo;
    glm::vec3 previous_position;
    enum GizmoType active_gizmo_type;

    // Cube for bounding box wireframe
    Cube bbox_wireframe;

    // Draw normals bool
    bool draw_normals;

    // Initialising functions
    bool init();
    void initObjects();
    void initGizmos();
    void initScene();

    // Updating functions
    void update();
    void runActions();

    // Rendering functions
    void render();
    void renderImGUI();

    // Pseudo initialising functions
    void addCube(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour,
                 float shininess);
    void addPlaceholderObject();
    void addPointLight(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour);

    // Pseudo updating functions
    void processMouseMovement(float mouse_xpos, float mouse_ypos);
    void processScreenResize(float new_window_x, float new_window_y);
    // Mouse intersection functions
    glm::vec3 mouseRaycast(float mouse_x, float mouse_y);
    void mouseObjectsIntersect(float mouse_x, float mouse_y);
    void mouseGizmosIntersect(float mouse_x, float mouse_y);
};
