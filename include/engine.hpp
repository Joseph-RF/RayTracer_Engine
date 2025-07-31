#pragma once

#include <vector>
#include <memory>

#include <glm/gtc/matrix_transform.hpp>

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

	Camera* active_camera;
private:
	float window_x;
	float window_y;

	Camera engine_camera;
	Camera raytracer_camera;

	Shader gameobject_shader;

	std::vector<std::unique_ptr<GameObject>> game_objects;
};