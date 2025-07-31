#pragma once

#include <shader.hpp>

class GameObject
{
public:
	glm::vec3 pos;
	glm::vec3 orientation;
	float size;
	glm::vec3 colour;

	virtual void draw(Shader& shader) = 0;
};