#pragma once

#include <iostream>

#include <glm/glm.hpp>

#include <aabb.hpp>

namespace Math {

bool rayPlaneIntersection(const glm::vec3& ray_origin, const glm::vec3& ray_direction,
                          const glm::vec3& plane_normal, const glm::vec3& plane_position, float& t);

glm::vec3 closestPointBetweenRays(const glm::vec3& ray1_origin, const glm::vec3& ray1_direction,
                                  const glm::vec3& ray2_origin, const glm::vec3& ray2_direction);

bool rayBoundingBoxIntersection(const glm::vec3& ray_origin, const glm::vec3& ray_direction,
                                const AABB bbox);
} // namespace Math
