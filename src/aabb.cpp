#include <aabb.hpp>

AABB::AABB()
	: xmin(0.0f), xmax(0.0f), ymin(0.0f), ymax(0.0f), zmin(0.0f), zmax(0.0f) {}

AABB::AABB(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
	: xmin(xmin), xmax(xmax), ymin(ymin), ymax(ymax), zmin(zmin), zmax(zmax) {}
