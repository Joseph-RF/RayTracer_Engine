#pragma once

#include <functional>

#include <arrow.hpp>
#include <cube.hpp>
#include <hollow_cylinder.hpp>
#include <math.hpp>

class Gizmo {
public:
    virtual void toggleActivity()                   = 0;
    virtual bool getActivity()                                 = 0;
    virtual void draw(Shader& shader)                     = 0;
    virtual void updatePosAndOrientation(std::shared_ptr<GameObject> target)         = 0;
    virtual void updateBoundingBox()                    = 0;
    virtual void transformation_function(glm::vec3& ray_origin, glm::vec3& ray_direction,
                                         std::shared_ptr<GameObject> target,
                                         glm::vec3& previous_pos, bool& using_gizmo) = 0;

    std::unique_ptr<GameObject> body;

protected:
    
    bool active;
    glm::vec3 position_offset;
    glm::vec3 orientation_offset;
    std::vector<glm::vec3> transformation_axes;
};

class AxisMoveGizmo : public Gizmo {
public:
    AxisMoveGizmo(std::string type);

    void toggleActivity() override;
    bool getActivity() override;
    void draw(Shader& shader) override;
    void updatePosAndOrientation(std::shared_ptr<GameObject> target) override; 
    void updateBoundingBox() override;
    void transformation_function(glm::vec3& ray_origin, glm::vec3& ray_direction,
                                 std::shared_ptr<GameObject> target, glm::vec3& previous_pos,
                                 bool& using_gizmo) override;
};

class PlaneMoveGizmo : public Gizmo {
public:
    PlaneMoveGizmo(std::string type);

    void toggleActivity() override;
    bool getActivity() override;
    void draw(Shader& shader) override;
    void updatePosAndOrientation(std::shared_ptr<GameObject> target) override;
    void updateBoundingBox() override;
    void transformation_function(glm::vec3& ray_origin, glm::vec3& ray_direction,
                                 std::shared_ptr<GameObject> target, glm::vec3& previous_pos,
                                 bool& using_gizmo) override;
};

class RotateGizmo : public Gizmo {
public:
    RotateGizmo(std::string type);

    void toggleActivity() override;
    bool getActivity() override;
    void draw(Shader& shader) override;
    void updatePosAndOrientation(std::shared_ptr<GameObject> target) override;
    void updateBoundingBox() override;
    void transformation_function(glm::vec3& ray_origin, glm::vec3& ray_direction,
                                 std::shared_ptr<GameObject> target, glm::vec3& previous_pos,
                                 bool& using_gizmo) override;
};