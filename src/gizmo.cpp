#include <gizmo.hpp>

AxisMoveGizmo::AxisMoveGizmo(std::string type) {
    glm::vec3 colour(0.0f, 0.0f, 0.0f);
    glm::vec3 scale(0.5f, 0.5f, 0.5f);
    glm::vec3 zero(0.0f, 0.0f, 0.0f);

    if (type == "X") {
        this->position_offset = glm::vec3(0.25f * Arrow::tail_height, 0.0, 0.0);
        this->orientation_offset  = glm::vec3(0.0, glm::radians(90.0f), 0.0);
        colour                   = glm::vec3(0.8f, 0.0f, 0.0f);

        transformation_axes.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    } else if (type == "Y") {
        this->position_offset = glm::vec3(0.0, 0.25f * Arrow::tail_height, 0.0);
        this->orientation_offset = glm::vec3(glm::radians(270.0f), 0.0, 0.0);
        colour                   = glm::vec3(0.0f, 0.8f, 0.0f);

        transformation_axes.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    } else if (type == "Z") {
        this->position_offset = glm::vec3(0.0, 0.0, 0.25f * Arrow::tail_height);
        this->orientation_offset = glm::vec3(0.0, 0.0, 0.0);
        colour                   = glm::vec3(0.0f, 0.0f, 0.8f);

        transformation_axes.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    } else {
        std::cout << "Incorrect axis move gizmo type given" << std::endl;
    }

    active = false;

    body = std::make_unique<Arrow>(zero, orientation_offset, scale, colour, 32.0f);
}

void AxisMoveGizmo::toggleActivity() {
    if (active) {
        active = false;
    } else {
        active = true;
    }
}

bool AxisMoveGizmo::getActivity() {
    return this->active;
}

void AxisMoveGizmo::draw(Shader& shader) {
    body->draw(shader);
}

void AxisMoveGizmo::updatePosAndOrientation(std::shared_ptr<GameObject> target) {
    // Update the position and orientation based off of the target object
    body->pos = target->pos + position_offset;
}


void AxisMoveGizmo::updateBoundingBox() {
    body->update_bounding_box();
}

void AxisMoveGizmo::transformation_function(glm::vec3& ray_origin, glm::vec3& ray_direction,
                                            std::shared_ptr<GameObject> target,
                                            glm::vec3& previous_pos, bool& using_gizmo) {
    glm::vec3 axis = this->transformation_axes[0];

    glm::vec3 closest_point_on_axis =
        Math::closestPointBetweenRays(this->body->pos, axis, ray_origin, ray_direction);

    if (!using_gizmo) {
        previous_pos = closest_point_on_axis;
        using_gizmo       = true;
    } else {
        target->pos += (closest_point_on_axis - previous_pos);
        previous_pos = closest_point_on_axis;
    }
}

PlaneMoveGizmo::PlaneMoveGizmo(std::string type) {
    glm::vec3 colour(0.0f, 0.0f, 0.0f);
    glm::vec3 scale(0.15, 0.15, 0.005);
    glm::vec3 zero(0.0f, 0.0f, 0.0f);

    if (type == "XY") {
        this->position_offset =
            glm::vec3(0.25f * Arrow::tail_height, 0.25f * Arrow::tail_height, 0.0);
        this->orientation_offset = glm::vec3(0.0f, 0.0f, 0.0f);
        colour                   = glm::vec3(0.8f, 0.8f, 0.0f);

        transformation_axes.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
        transformation_axes.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    } else if (type == "XZ") {
        this->position_offset =
            glm::vec3(0.25f * Arrow::tail_height, 0.0, 0.25f * Arrow::tail_height);
        this->orientation_offset = glm::vec3(glm::radians(90.0f), 0.0, 0.0);
        colour                   = glm::vec3(0.8f, 0.0f, 0.8f);

        transformation_axes.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
        transformation_axes.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    } else if (type == "YZ") {
        this->position_offset =
            glm::vec3(0.0, 0.25f * Arrow::tail_height, 0.25f * Arrow::tail_height);
        this->orientation_offset = glm::vec3(0.0, glm::radians(90.0f), 0.0);
        colour                   = glm::vec3(0.0f, 0.8f, 0.8f);

        transformation_axes.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        transformation_axes.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    } else {
        std::cout << "Incorrect plane move gizmo type given" << std::endl;
    }

    active = false;

    body = std::make_unique<Cube>(zero, orientation_offset, scale, colour, 32.0f);
}

void PlaneMoveGizmo::toggleActivity() {
    if (active) {
        active = false;
    } else {
        active = true;
    }
}

bool PlaneMoveGizmo::getActivity() {
    return this->active;
}

void PlaneMoveGizmo::draw(Shader& shader) {
    body->draw(shader);
}

void PlaneMoveGizmo::updatePosAndOrientation(std::shared_ptr<GameObject> target) {
    // Update the position and orientation based off of the target object
    body->pos = target->pos + position_offset;
}

void PlaneMoveGizmo::updateBoundingBox() {
    body->update_bounding_box();
}

void PlaneMoveGizmo::transformation_function(glm::vec3& ray_origin, glm::vec3& ray_direction,
                                             std::shared_ptr<GameObject> target,
                                             glm::vec3& previous_pos, bool& using_gizmo) {
    float t = 0.0; // Ray from camera to mouse parameter

    glm::vec3 axis1 = this->transformation_axes[0];
    glm::vec3 axis2 = this->transformation_axes[1];

    glm::vec3 plane_normal = glm::normalize(glm::cross(axis1, axis2));

    if (!Math::rayPlaneIntersection(ray_origin, ray_direction, plane_normal, this->body->pos,
                                    t)) {
        return;
    }

    glm::vec3 plane_intersection = ray_origin + ray_direction * t;

    if (!using_gizmo) {
        previous_pos = plane_intersection;
        using_gizmo       = true;
    } else {
        target->pos += (plane_intersection - previous_pos);
        previous_pos = plane_intersection;
    }
}

RotateGizmo::RotateGizmo(std::string type) {
    glm::vec3 colour(0.0f, 0.0f, 0.0f);
    glm::vec3 scale(1.0f, 1.0f, 1.0f);
    glm::vec3 zero(0.0f, 0.0f, 0.0f);

    if (type == "X") {
        this->position_offset    = zero;
        this->orientation_offset = glm::vec3(0.0, glm::radians(90.0f), 0.0);
        colour                   = glm::vec3(0.8f, 0.0f, 0.0f);
        scale                    = glm::vec3(0.8f, 0.8f, 0.05f);

        transformation_axes.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        transformation_axes.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
        transformation_axes.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
    } else if (type == "Y") {
        this->position_offset    = zero;
        this->orientation_offset = glm::vec3(glm::radians(90.0f), 0.0, 0.0);
        colour                   = glm::vec3(0.0f, 0.8f, 0.0f);
        scale                    = glm::vec3(0.9f, 0.9f, 0.05f);

        transformation_axes.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
        transformation_axes.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
        transformation_axes.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    } else if (type == "Z") {
        this->position_offset    = zero;
        this->orientation_offset = glm::vec3(0.0, 0.0, 0.0);
        colour                   = glm::vec3(0.0f, 0.0f, 0.8f);
        scale                    = glm::vec3(1.0f, 1.0f, 0.05f);

        transformation_axes.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
        transformation_axes.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        transformation_axes.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
    } else {
        std::cout << "Incorrect rotate gizmo type given" << std::endl;
    }

    active = false;

    body = std::make_unique<HollowCylinder>(zero, zero, scale, colour, 32.0f);
}

void RotateGizmo::toggleActivity() {
    if (active) {
        active = false;
    } else {
        active = true;
    }
}

bool RotateGizmo::getActivity() {
    return this->active;
}


void RotateGizmo::draw(Shader& shader) {
    body->draw(shader);
}

void RotateGizmo::updatePosAndOrientation(std::shared_ptr<GameObject> target) {
    // Update the position and orientation based off of the target object
    body->pos         = target->pos + position_offset;
    body->orientation = target->orientation * (transformation_axes[0] + transformation_axes[1]) +
                        orientation_offset; 
}

void RotateGizmo::updateBoundingBox() {
    body->update_bounding_box();
}

void RotateGizmo::transformation_function(glm::vec3& ray_origin, glm::vec3& ray_direction,
                                          std::shared_ptr<GameObject> target,
                                          glm::vec3& previous_pos, bool& using_gizmo) {
    float t = 0.0; // Ray from camera to mouse parameter

    glm::vec3 axis1         = this->transformation_axes[0];
    glm::vec3 axis2         = this->transformation_axes[1];
    glm::vec3 rotation_axis = this->transformation_axes[2];

    glm::vec3 plane_normal = glm::normalize(glm::cross(axis1, axis2));

    if (!Math::rayPlaneIntersection(ray_origin, ray_direction, plane_normal, this->body->pos,
                                    t)) {
        return;
    }

    glm::vec3 plane_intersection = ray_origin + ray_direction * t;

    if (!using_gizmo) {
        previous_pos = plane_intersection;
        using_gizmo       = true;
        return;
    }

    glm::vec3 a   = previous_pos - target->pos;
    glm::vec3 b   = plane_intersection - target->pos;
    float det_abu = glm::dot(glm::cross(a, b), rotation_axis);

    float angle = std::acos(glm::dot(a, b) / (glm::length(a) * glm::length(b)));

    if (det_abu < 0) {
        angle *= -1;
    }

    target->orientation += angle * rotation_axis;

    previous_pos = plane_intersection;
}
