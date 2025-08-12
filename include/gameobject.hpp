#pragma once

#include <string>
#include <memory>

#include <shader.hpp>
#include <aabb.hpp>
#include <light.hpp>

class GameObject
{
public:
	glm::vec3 pos;
	glm::vec3 orientation;
	float size;
	glm::vec3 colour;
	float shininess;

	AABB bbox;
	std::string name;

	std::unique_ptr<Light> light;

	virtual void draw(Shader& shader) = 0;
	virtual AABB update_bounding_box() = 0;
	virtual void add_light(
		float ambient,
		float diffuse,
		float specular,
		float constant,
		float linear,
		float quadratic
	) = 0;

private:

};