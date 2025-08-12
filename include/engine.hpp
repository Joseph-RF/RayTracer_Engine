#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <map>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <camera.hpp>
#include <shader.hpp>
#include <cube.hpp>

class Engine
{
public:
	Engine(float window_x, float window_y);

	void init();
	void update();
	void render();
	void render_imgui();

	void mouseObjectsIntersect(float mouse_x, float mouse_y);
	glm::vec3 mouseRaycast(float mouse_x, float mouse_y);

	bool mouseIntersectsBoundingBox(
		glm::vec3 mouse_direction,
		std::shared_ptr<GameObject> object
	);

	void processMouseClick();

	void set_window_size(float window_x, float window_y);

	Camera* active_camera;
private:
	void addCube(
		glm::vec3 pos,
		glm::vec3 orientation,
		float size,
		glm::vec3 colour,
		float shininess
	);

	void update_shader_lights(Shader& shader);

	float window_x;
	float window_y;

	Camera engine_camera;
	Camera raytracer_camera;

	Shader gameobject_shader;
	Shader outline_shader;
	Shader light_shader;

	std::shared_ptr<GameObject> mouseover_object;
	std::shared_ptr<GameObject> selected_object;

	std::vector<std::shared_ptr<GameObject>> game_objects;
	std::map<std::string, unsigned int> game_objects_name_map;

	// Data for placeholder object
	glm::vec3 placeholder_pos;
	glm::vec3 placeholder_orientation;
	float placeholder_size;
	glm::vec3 placeholder_colour;
	float placeholder_shininess;

	Light placeholder_light;

	unsigned int num_lights;
	int max_lights;
};