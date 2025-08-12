#pragma once

class Light {
public:
    Light();

    Light(
        float ambient,
        float diffuse,
        float specular,
        float constant,
        float linear,
        float quadratic
    );

    float ambient;
    float diffuse;
    float specular;
    float constant;
    float linear;
    float quadratic;
};
