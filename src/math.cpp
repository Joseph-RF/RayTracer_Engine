#include "math.hpp"

bool Math::rayPlaneIntersection(const glm::vec3& ray_origin, const glm::vec3& ray_direction,
                          const glm::vec3& plane_normal, const glm::vec3& plane_position,
                          float& t) {
    // Ray direction and Plane Normal MUST be normalised
    float denom = glm::dot(plane_normal, ray_direction);
    if (std::abs(denom) > 1e-6) {
        glm::vec3 p0_l0 = plane_position - ray_origin;
        t               = glm::dot(p0_l0, plane_normal) / denom;
        return (t >= 0);
    }
    return false;
}

glm::vec3 Math::closestPointBetweenRays(const glm::vec3& ray1_origin, const glm::vec3& ray1_direction,
                                  const glm::vec3& ray2_origin, const glm::vec3& ray2_direction) {
    // Returns the closest point between ray1 and ray2.
    // Closest point lies on ray1.
    // Directions MUST be normalised

    glm::vec3 s_d = ray1_origin - ray2_origin;

    float dadb  = glm::dot(ray1_direction, ray2_direction);
    float dasd  = glm::dot(ray1_direction, s_d);
    float dbsd  = glm::dot(ray2_direction, s_d);
    float denom = 1.0f - dadb * dadb;

    if (denom == 0.0) {
        std::cout << "rays are parallel" << std::endl;
        return glm::vec3(0.0, 0.0, 0.0);
    }

    return ((-dasd + dadb * dbsd) / denom) * ray1_direction + ray1_origin;
}

bool Math::rayBoundingBoxIntersection(const glm::vec3& ray_origin, const glm::vec3& ray_direction,
                                      const AABB bbox) {
    glm::vec3 dirfrac;
    dirfrac.x = 1.0f / ray_direction.x;
    dirfrac.y = 1.0f / ray_direction.y;
    dirfrac.z = 1.0f / ray_direction.z;

    float t1 = (bbox.xmin - ray_origin.x) * dirfrac.x;
    float t2 = (bbox.xmax - ray_origin.x) * dirfrac.x;
    float t3 = (bbox.ymin - ray_origin.y) * dirfrac.y;
    float t4 = (bbox.ymax - ray_origin.y) * dirfrac.y;
    float t5 = (bbox.zmin - ray_origin.z) * dirfrac.z;
    float t6 = (bbox.zmax - ray_origin.z) * dirfrac.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0) {
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax) {
        return false;
    }

    return true;
}