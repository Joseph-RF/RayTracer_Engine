#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <arrow.hpp>
#include <camera.hpp>
#include <cube.hpp>
#include <hollow_cylinder.hpp>
#include <scenesaver.hpp>
#include <shader.hpp>
#include <sphere.hpp>

enum GizmoType { MOVE, ROTATE };

class Engine {
public:
    Engine(float window_x, float window_y);

    void init();
    void update();
    void render();
    void render_imgui();

    void processMouseMovement(float mouse_x, float mouse_y);
    void processMouseClick();

    void setActiveGizmoType(enum GizmoType gizmo_type);

    void setWindowSize(float window_x, float window_y);

    void resetObjectPointers();

    Camera* active_camera;

    std::vector<std::shared_ptr<GameObject>> game_objects;
    unsigned int num_lights;

    bool mouse_pressed;

private:
    void addCube(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour,
                 float shininess);
    void addPointLight(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour);

    // Updating functions
    void updateShaderLights(Shader& shader);

    // Mouse intersection functions
    void mouseObjectsIntersect(float mouse_x, float mouse_y);
    void mouseGizmosIntersect(float mouse_x, float mouse_y);

    glm::vec3 mouseRaycast(float mouse_x, float mouse_y);

    bool mouseIntersectsBoundingBox(glm::vec3 mouse_direction, AABB bbox);
    bool rayPlaneIntersection(glm::vec3& ray_origin, glm::vec3& ray_direction,
                              glm::vec3& plane_normal, glm::vec3& plane_position, float& t);
    glm::vec3 closestPointBetweenRays(glm::vec3& ray1_origin, glm::vec3& ray1_direction,
                                      glm::vec3& ray2_origin, glm::vec3& ray2_direction);

    // Gizmo functions
    void gizmoAxisMoveFunction(glm::vec3& mouse_ray, std::shared_ptr<GameObject> gizmo);
    void gizmoPlaneFunction(glm::vec3& mouse_ray, std::shared_ptr<GameObject> gizmo);
    void gizmoRotationFunction(glm::vec3& mouse_ray, std::shared_ptr<GameObject> gizmo);

    // Rendering functions
    void renderOutlinedObject();
    void renderBbox(std::shared_ptr<GameObject> game_object, glm::mat4& view,
                    glm::mat4& projection);
    void renderMoveGizmos(std::shared_ptr<GameObject> game_object, glm::mat4& view,
                          glm::mat4& projection);

    float window_x;
    float window_y;

    Camera engine_camera;
    Camera raytracer_camera;

    Shader gameobject_shader;
    Shader outline_shader;
    Shader light_shader;

    std::shared_ptr<GameObject> mouseover_object;
    std::shared_ptr<GameObject> selected_object;

    // Gizmos
    std::shared_ptr<GameObject> active_gizmo;
    std::shared_ptr<Sphere> centre_gizmo;

    std::shared_ptr<Arrow> x_arrow;
    std::shared_ptr<Arrow> y_arrow;
    std::shared_ptr<Arrow> z_arrow;

    std::shared_ptr<Cube> xy_plane_gizmo;
    std::shared_ptr<Cube> xz_plane_gizmo;
    std::shared_ptr<Cube> yz_plane_gizmo;

    std::shared_ptr<HollowCylinder> x_rotation_gizmo;
    std::shared_ptr<HollowCylinder> y_rotation_gizmo;
    std::shared_ptr<HollowCylinder> z_rotation_gizmo;

    // Gizmo function utility
    bool using_gizmo;
    glm::vec3 previous_position;
    enum GizmoType active_gizmo_type;

    // Data for placeholder object
    glm::vec3 placeholder_pos;
    glm::vec3 placeholder_orientation;
    glm::vec3 placeholder_scale;
    glm::vec3 placeholder_colour;
    float placeholder_shininess;

    Light placeholder_light;

    // Cube for bounding box wireframe
    Cube bbox_wireframe;

    int max_lights;
};
