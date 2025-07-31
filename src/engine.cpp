#include <engine.hpp>

Engine::Engine(float window_x, float window_y) : window_x(window_x), window_y(window_y) {
	active_camera = &engine_camera;
}

void Engine::init() {
	gameobject_shader = Shader(RESOURCES_PATH "vshader.glsl", RESOURCES_PATH "fshader.glsl");

	// Initialise the cube class
	Cube::init();

	game_objects.push_back(std::make_unique<Cube>(
		glm::vec3(1.0, 1.0, 1.0),
		glm::vec3(0.0, 0.0, 0.0),
		1.0,
		glm::vec3(0.0, 0.8, 0.0))
	);
}

void Engine::update() {

}

void Engine::render() {

	// View and projection matrices won't change between objects
	glm::mat4 view(1.0);
	glm::mat4 projection(1.0);
	
	view = active_camera->lookAt();
	projection = glm::perspective(glm::radians(active_camera->fov), (window_x / window_y), 0.1f, 100.f);

	gameobject_shader.setMat("view", view);
	gameobject_shader.setMat("projection", projection);

	for (unsigned int i = 0; i < game_objects.size(); ++i) {
		game_objects[i]->draw(gameobject_shader);
	}
}