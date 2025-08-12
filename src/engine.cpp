#include <engine.hpp>

Engine::Engine(float window_x, float window_y) : window_x(window_x), window_y(window_y) {
	active_camera = &engine_camera;
	mouseover_object = NULL;
	selected_object = NULL;

	placeholder_pos = glm::vec3(0.0, 0.0, 0.0);
	placeholder_orientation = glm::vec3(0.0, 0.0, 0.0);
	placeholder_size = 1.0f;
	placeholder_colour = glm::vec3(0.0, 0.0, 0.0);

	placeholder_light = Light(0.1, 0.8, 1.0, 1.0, 0.09, 0.032);

	max_lights = 16;
	num_lights = 0;
}

void Engine::init() {
	gameobject_shader = Shader(RESOURCES_PATH "vshader.glsl", RESOURCES_PATH "fshader.glsl");
	outline_shader = Shader(RESOURCES_PATH "outline_vshader.glsl", RESOURCES_PATH "outline_fshader.glsl");
	light_shader = Shader(RESOURCES_PATH "vshader.glsl", RESOURCES_PATH "light_fshader.glsl");

	// Initialise the cube class
	Cube::init();

	addCube(
		glm::vec3(8.0, -2.0, 1.0),
		glm::vec3(0.0, 0.0, 0.0),
		1.0,
		glm::vec3(0.0, 0.8, 0.0),
		32.0
	);
	addCube(
		glm::vec3(-2.0, -2.0, 1.0),
		glm::vec3(0.0, 0.0, 0.0),
		1.0,
		glm::vec3(0.0, 0.0, 0.6),
		32.0
	);
}

void Engine::update() {
	// Update the bounding box of the currently selected object
	if (selected_object) {
		selected_object->update_bounding_box();
	}
}

void Engine::render() {

	// View and projection matrices won't change between objects
	glm::mat4 view = active_camera->lookAt();
	glm::mat4 projection = glm::perspective(
		glm::radians(active_camera->fov), (window_x / window_y), 0.1f, 100.f
	);

	gameobject_shader.use();
	gameobject_shader.setMat("view", view);
	gameobject_shader.setMat("projection", projection);
	update_shader_lights(gameobject_shader);
	gameobject_shader.setInt("point_lights_number", num_lights);

	outline_shader.use();
	outline_shader.setMat("view", view);
	outline_shader.setMat("projection", projection);

	light_shader.use();
	light_shader.setMat("view", view);
	light_shader.setMat("projection", projection);

	for (int i = 0; i < game_objects.size(); ++i) {
		
		if (game_objects[i] == selected_object || game_objects[i] == mouseover_object) {
			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		}
		
		// Draw lights one way, objects in another
		if (game_objects[i]->light) {
			game_objects[i]->draw(light_shader);
		}
		else {
			game_objects[i]->draw(gameobject_shader);
		}


		if (game_objects[i] == selected_object || game_objects[i] == mouseover_object) {
			game_objects[i]->size *= 1.05;
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);

			glDisable(GL_DEPTH_TEST);

			game_objects[i]->draw(outline_shader);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			glEnable(GL_DEPTH_TEST);
			glStencilMask(0xFF);
			game_objects[i]->size /= 1.05;
		}
	}
}

void Engine::render_imgui() {
	ImGui::Begin("ImGUI Window", NULL);

	ImGui::BeginTabBar("Settings#left_tabs_bar");
	// Object editor tab
	// -------------------------------------
	bool boolean = true;
	if (ImGui::BeginTabItem("Object Editor", &boolean, ImGuiTabItemFlags_None)) {
		ImGui::Text("This is the object editor tab");

		if (selected_object) {
			ImGui::SliderFloat(
				"Selected object x position", &selected_object->pos.x, -50, 50
			);
			ImGui::SliderFloat(
				"Selected object y position", &selected_object->pos.y, -50, 50
			);
			ImGui::SliderFloat(
				"Selected object z position", &selected_object->pos.z, -50, 50
			);
			ImGui::SliderFloat(
				"Selected object x orientation", &selected_object->orientation.x, -3.14, 3.14
			);
			ImGui::SliderFloat(
				"Selected object y orientation", &selected_object->orientation.y, -3.14, 3.14
			);
			ImGui::SliderFloat(
				"Selected object z orientation", &selected_object->orientation.z, -3.14, 3.14
			);
			ImGui::SliderFloat(
				"Selected object scale", &selected_object->size, 0.1, 10
			);
			ImGui::ColorEdit3(
				"Selected object colour", glm::value_ptr(selected_object->colour)
			);

			if (selected_object->light) {
				ImGui::SliderFloat(
					"Selected light ambient factor",
					&selected_object->light->ambient, 0, 1
				);
				ImGui::SliderFloat(
					"Selected light diffuse factor",
					&selected_object->light->diffuse, 0, 1
				);
				ImGui::SliderFloat(
					"Selected light specular factor",
					&selected_object->light->specular, 0, 1
				);
				ImGui::SliderFloat(
					"Selected light constant attenuation factor",
					&selected_object->light->constant, 0, 4
				);
				ImGui::SliderFloat(
					"Selected light linear attenuation factor",
					&selected_object->light->linear, 0, 0.5
				);
				ImGui::SliderFloat(
					"Selected light quadratic attenuation factor",
					&selected_object->light->quadratic, 0, 0.1
				);
			}

			/*
			char temp_name[255] = "";
			ImGui::InputTextWithHint("Object Name", selected_object->name.c_str(), temp_name, IM_ARRAYSIZE(temp_name));
			ImGui::LabelText(selected_object->name.c_str(), "Game Object");
			selected_object->name = temp_name;
			*/
		}

		ImGui::EndTabItem();
	}

	// Objects selector tab
	// -------------------------------------
	if (ImGui::BeginTabItem("Objects Selector", &boolean, ImGuiTabItemFlags_None)) {
		ImGui::Text("This is the objects selector tab");

		auto it = game_objects.begin();
		while (it != game_objects.end()) {
			std::string object_name = (*it)->name;
			if (ImGui::Button(object_name.c_str())) {
				selected_object = *it;
				mouseover_object = NULL;
			}
			ImGui::SameLine();
			std::string button_id = "Delete Object##";
			button_id += object_name;
			if (ImGui::Button(button_id.c_str())) {
				if (selected_object == *it) {
					selected_object = NULL;
				}
				it = game_objects.erase(it);
			}
			else {
				it++;
			}
		}
		ImGui::EndTabItem();
	}

	// Object adder tab
	// -------------------------------------
	if (ImGui::BeginTabItem("Add Objects", &boolean, ImGuiTabItemFlags_None)) {
		ImGui::Text("Add objects to the scene here");

		// Note: Will have to refactor this if objects properties change

		ImGui::SliderFloat("Selected object x position", &placeholder_pos.x, -50, 50);
		ImGui::SliderFloat("Selected object y position", &placeholder_pos.y, -50, 50);
		ImGui::SliderFloat("Selected object z position", &placeholder_pos.z, -50, 50);
		ImGui::SliderFloat("Selected object x orientation", &placeholder_orientation.x, -3.14, 3.14);
		ImGui::SliderFloat("Selected object y orientation", &placeholder_orientation.y, -3.14, 3.14);
		ImGui::SliderFloat("Selected object z orientation", &placeholder_orientation.z, -3.14, 3.14);
		ImGui::SliderFloat("Selected object scale", &placeholder_size, 0.1, 10);
		ImGui::SliderFloat("Selected object shininess", &placeholder_shininess, 0.1, 64);
		ImGui::ColorEdit3("Selected object colour", glm::value_ptr(placeholder_colour));

		if (ImGui::Button("Add Object")) {
			addCube(
				placeholder_pos,
				placeholder_orientation,
				placeholder_size,
				placeholder_colour,
				placeholder_shininess
			);
			selected_object = game_objects[game_objects.size() - 1];
			mouseover_object = NULL;
		}

		ImGui::Text("Configure the parameters below to add a cube shaped light");

		ImGui::SliderFloat(
			"Selected light ambient factor",
			&placeholder_light.ambient, 0, 1
		);
		ImGui::SliderFloat(
			"Selected light diffuse factor",
			&placeholder_light.diffuse, 0, 1
		);
		ImGui::SliderFloat(
			"Selected light specular factor",
			&placeholder_light.specular, 0, 1
		);
		ImGui::SliderFloat(
			"Selected light constant attenuation factor",
			&placeholder_light.constant, 0, 4
		);
		ImGui::SliderFloat(
			"Selected light linear attenuation factor",
			&placeholder_light.linear, 0, 0.5
		);
		ImGui::SliderFloat(
			"Selected light quadratic attenuation factor",
			&placeholder_light.quadratic, 0, 0.1
		);

		if (ImGui::Button("Add Light Object")) {
			if (num_lights >= max_lights) {
				std::cout<<"Max number of lights reached"<<std::endl;
			} else {
				unsigned int n = game_objects.size();
				addCube(
					placeholder_pos,
					placeholder_orientation,
					placeholder_size,
					placeholder_colour,
					placeholder_shininess
				);

				game_objects[n]->add_light(
					placeholder_light.ambient,
					placeholder_light.diffuse,
					placeholder_light.specular,
					placeholder_light.constant,
					placeholder_light.linear,
					placeholder_light.quadratic
				);

				num_lights++;

				selected_object = game_objects[n];
				mouseover_object = NULL;
			}
		}

		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
	ImGui::End();
}

void Engine::mouseObjectsIntersect(float mouse_x, float mouse_y) {
	glm::vec3 mouse_direction = mouseRaycast(mouse_x, mouse_y);

	// Keep track of whether any object is under the mouse
	bool mouseover = false;

	for (unsigned int i = 0; i < game_objects.size(); ++i) {
		if (mouseIntersectsBoundingBox(mouse_direction, game_objects[i])) {
			if (selected_object == game_objects[i] && selected_object) {
				// Ignore this check if the item currently being hovered over is the
				// selected object. Ensure selected_object is an actual object and not
				// just NULL
				continue;
			}
			mouseover = true;
			mouseover_object = game_objects[i];
		}
	}
	if (!mouseover) {
		// If no object has mouse over it, set mouseover_object to NULL
		mouseover_object = NULL;
	}
}

glm::vec3 Engine::mouseRaycast(float mouse_x, float mouse_y) {
	// Mouse position in x and y with it's z at the very far end of the 
	// NDC space
	glm::vec3 mouse_ndc(
		(2.0f * mouse_x - window_x) / window_x,
		(window_y - 2.0f * mouse_y) / window_y,
		1.0f
	);
	
	// Mouse in clip space with x and y, z is set at just one unit in front of the camera
	// w coordinate set to 1 to keep things simple
	glm::vec4 mouse_clip = glm::vec4(mouse_ndc.x, mouse_ndc.y, 1.0, 1.0);

	glm::mat4 projection = glm::perspective(
		glm::radians(active_camera->fov), (window_x / window_y), 0.1f, 100.f
	);

	glm::mat4 view = active_camera->lookAt();
	
	// Find position of the mouse in world space coordinates using the inverse
	// transformation of clip space -> world space
	glm::vec4 mouse_world = glm::inverse(projection * view) * mouse_clip;
	mouse_world /= mouse_world.w;

	// Find the direction of the vector pointing FROM the camera TO the mouse
	glm::vec3 mouse_ray_direction = glm::normalize(glm::vec3(mouse_world) - active_camera->pos);

	return mouse_ray_direction;
}

bool Engine::mouseIntersectsBoundingBox(
	glm::vec3 mouse_direction,
	std::shared_ptr<GameObject> object
) {

	glm::vec3 dirfrac;
	dirfrac.x = 1.0f / mouse_direction.x;
	dirfrac.y = 1.0f / mouse_direction.y;
	dirfrac.z = 1.0f / mouse_direction.z;

	AABB bbox = object->bbox;
	
	float t1 = (object->bbox.xmin - active_camera->pos.x) * dirfrac.x;
	float t2 = (object->bbox.xmax - active_camera->pos.x) * dirfrac.x;
	float t3 = (object->bbox.ymin - active_camera->pos.y) * dirfrac.y;
	float t4 = (object->bbox.ymax - active_camera->pos.y) * dirfrac.y;
	float t5 = (object->bbox.zmin - active_camera->pos.z) * dirfrac.z;
	float t6 = (object->bbox.zmax - active_camera->pos.z) * dirfrac.z;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
	if (tmax < 0){
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax) {
		return false;
	}

	return true;
}

void Engine::processMouseClick() {
	if (mouseover_object) {
		selected_object = mouseover_object;
		mouseover_object = NULL;
	}
}

void Engine::set_window_size(float window_x, float window_y) {
	this->window_x = window_x;
	this->window_y = window_y;
}

void Engine::addCube(
	glm::vec3 pos,
	glm::vec3 orientation,
	float size,
	glm::vec3 colour,
	float shininess
) {
	// Use this number to name the object (cube)
	unsigned int n = game_objects.size();

	game_objects.push_back(
		std::make_shared<Cube>(pos, orientation, size, colour, shininess)
	);

	game_objects[n]->name = "Object_" + std::to_string(n);
}

void Engine::update_shader_lights(Shader &shader) {
	shader.use();
	unsigned int counter = 0;

	for (unsigned int i = 0; i < game_objects.size(); ++i) {

		if (counter > max_lights) {
			std::cout << "Counter somehow greater than number of max light" << std::endl;
		}

		if (game_objects[i]->light) {
			shader.setVec3(
				"point_lights[" + std::to_string(counter) + "].position",
				game_objects[i]->pos
			);
			shader.setVec3(
				"point_lights[" + std::to_string(counter) + "].colour",
				game_objects[i]->colour
			);
			shader.setFloat(
				"point_lights[" + std::to_string(counter) + "].ambient",
				game_objects[i]->light->ambient
			);
			shader.setFloat(
				"point_lights[" + std::to_string(counter) + "].diffuse",
				game_objects[i]->light->diffuse
			);
			shader.setFloat(
				"point_lights[" + std::to_string(counter) + "].specular",
				game_objects[i]->light->specular
			);
			shader.setFloat(
				"point_lights[" + std::to_string(counter) + "].constant",
				game_objects[i]->light->constant
			);
			shader.setFloat(
				"point_lights[" + std::to_string(counter) + "].linear",
				game_objects[i]->light->linear
			);
			shader.setFloat(
				"point_lights[" + std::to_string(counter) + "].quadratic",
				game_objects[i]->light->quadratic
			);

			counter++;
		}
	}
}
