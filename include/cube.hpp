#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <gameobject.hpp>

class Cube : public GameObject
{
public:
	Cube(
		glm::vec3 pos,
		glm::vec3 orientation,
		float size,
		glm::vec3 colour,
		float shininess
	);

	void draw(Shader& shader) override;

	AABB update_bounding_box() override;

	void add_light(
		float ambient,
		float diffuse,
		float specular,
		float constant,
		float linear,
		float quadratic
	) override;

	static void init();

private:

	static unsigned int VBO;
	static unsigned int EBO;
	static unsigned int VAO;
};