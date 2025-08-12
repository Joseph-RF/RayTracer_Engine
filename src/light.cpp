#include <light.hpp>

Light::Light() : ambient(0.0f), diffuse(0.0f), specular(0.0f),
                 constant(0.0f), linear(0.0f), quadratic(0.0f) {}

Light::Light(
    float ambient,
    float diffuse,
    float specular,
    float constant,
    float linear,
    float quadratic
) : ambient(ambient), diffuse(diffuse), specular(specular),
    constant(constant), linear(linear), quadratic(quadratic) {}