#include <engine.hpp>

Engine::Engine(float window_x, float window_y) : window_x(window_x), window_y(window_y) {
	active_camera = &engine_camera;
	selected_object = NULL;
}

void Engine::init() {
	gameobject_shader = Shader(RESOURCES_PATH "vshader.glsl", RESOURCES_PATH "fshader.glsl");

	// Initialise the cube class
	Cube::init();

	game_objects.push_back(std::make_shared<Cube>(
		glm::vec3(8.0, -2.0, 1.0),
		glm::vec3(0.0, 0.0, 0.0),
		1.0,
		glm::vec3(0.0, 0.8, 0.0))
	);
	selected_object = game_objects[0];
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

void Engine::render_imgui() {
	ImGui::Begin("ImGUI Window", NULL);
	ImGui::Text("This is an ImGUI window!");
	if (selected_object) {
		ImGui::SliderFloat("##Selected object x position", &selected_object->pos.x, -50, 50);
		ImGui::SliderFloat("##Selected object y position", &selected_object->pos.y, -50, 50);
		ImGui::SliderFloat("##Selected object z position", &selected_object->pos.z, -50, 50);
		ImGui::SliderFloat("##Selected object x orientation", &selected_object->orientation.x, -3.14, 3.14);
		ImGui::SliderFloat("##Selected object y orientation", &selected_object->orientation.y, -3.14, 3.14);
		ImGui::SliderFloat("##Selected object z orientation", &selected_object->orientation.z, -3.14, 3.14);
		ImGui::SliderFloat("##Selected object scale", &selected_object->size, 0.1, 10);
		ImGui::ColorEdit3("Selected object colour", glm::value_ptr(selected_object->colour));
	}
	ImGui::End();
}