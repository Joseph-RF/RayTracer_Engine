#pragma once

#include <vector>
#include <memory>
#include <algorithm>

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

	Camera* active_camera;
private:
	float window_x;
	float window_y;

	Camera engine_camera;
	Camera raytracer_camera;

	Shader gameobject_shader;
	Shader outline_shader;

	std::shared_ptr<GameObject> mouseover_object;
	std::shared_ptr<GameObject> selected_object;

	std::vector<std::shared_ptr<GameObject>> game_objects;
};