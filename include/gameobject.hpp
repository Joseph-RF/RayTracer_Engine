#pragma once

#include <vector>
#include <string>

#include <shader.hpp>
#include <aabb.hpp>

class GameObject
{
public:
	glm::vec3 pos;
	glm::vec3 orientation;
	float size;
	glm::vec3 colour;

	AABB bbox;
	std::string name;

	virtual void draw(Shader& shader) = 0;
	virtual AABB update_bounding_box() = 0;

private:

};