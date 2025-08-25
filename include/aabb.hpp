#pragma once

class AABB {
public:
    AABB();
    AABB(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);

    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;

private:
};
