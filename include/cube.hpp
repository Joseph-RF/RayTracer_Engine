#pragma once

#include <glm/glm.hpp>

#include <glad/glad.h>

#include <gameobject.hpp>

class Cube : public GameObject
{
public:
	Cube(glm::vec3 pos, glm::vec3 orientation, float size, glm::vec3 colour);

	void draw(Shader& shader) override;

	AABB update_bounding_box() override;

	static void init();

private:

	static unsigned int VBO;
	static unsigned int EBO;
	static unsigned int VAO;
};