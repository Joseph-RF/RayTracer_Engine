#pragma once

#include <vector>
#include <memory>

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

	Camera* active_camera;
private:
	float window_x;
	float window_y;

	Camera engine_camera;
	Camera raytracer_camera;

	Shader gameobject_shader;

	std::shared_ptr<GameObject> selected_object;
	std::vector<std::shared_ptr<GameObject>> game_objects;
};