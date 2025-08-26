#include <engine.hpp>

// Public functions
// --------------------------------------------------------------------------------------

Engine::Engine(float window_x, float window_y)
    : window_x(window_x)
    , window_y(window_y) {
    active_camera    = &engine_camera;
    mouseover_object = nullptr;
    selected_object  = nullptr;
    active_gizmo     = nullptr;

    centre_gizmo = std::make_shared<Sphere>();

    x_arrow = std::make_shared<Arrow>();
    y_arrow = std::make_shared<Arrow>();
    z_arrow = std::make_shared<Arrow>();

    xy_plane_gizmo = std::make_shared<Cube>();
    xz_plane_gizmo = std::make_shared<Cube>();
    yz_plane_gizmo = std::make_shared<Cube>();

    x_rotation_gizmo = std::make_shared<HollowCylinder>();
    y_rotation_gizmo = std::make_shared<HollowCylinder>();
    z_rotation_gizmo = std::make_shared<HollowCylinder>();

    using_gizmo       = false;
    previous_position = glm::vec3(0.0, 0.0, 0.0);
    active_gizmo_type = MOVE;
    mouse_pressed     = false;

    placeholder_pos         = glm::vec3(0.0, 0.0, 0.0);
    placeholder_orientation = glm::vec3(0.0, 0.0, 0.0);
    placeholder_scale       = glm::vec3(1.0, 1.0, 1.0);
    placeholder_colour      = glm::vec3(0.0, 0.0, 0.0);
    placeholder_shininess   = 32.0f;

    placeholder_light = Light(0.1, 0.8, 1.0, 1.0, 0.09, 0.032);

    max_lights = 16;
    num_lights = 0;
}

void Engine::init() {
    gameobject_shader = Shader(RESOURCES_PATH "vshader.glsl", RESOURCES_PATH "fshader.glsl");
    outline_shader =
        Shader(RESOURCES_PATH "outline_vshader.glsl", RESOURCES_PATH "outline_fshader.glsl");
    light_shader = Shader(RESOURCES_PATH "vshader.glsl", RESOURCES_PATH "light_fshader.glsl");

    // Initialise Cube static variables that need OpenGL to exist
    Cube::init();
    // Initialise Arrow static variables
    Arrow::init();
    // Initialise sphere
    Sphere::init();
    // Initialise hollow cylinder
    HollowCylinder::init();

    centre_gizmo->colour = glm::vec3(0.8, 0.8, 0.8);
    centre_gizmo->scale  = glm::vec3(0.05, 0.05, 0.05);
    centre_gizmo->name   = "CENTRE_GIZMO";

    // Set arrow gizmo properties
    x_arrow->colour = glm::vec3(0.8, 0.0, 0.0);
    y_arrow->colour = glm::vec3(0.0, 0.8, 0.0);
    z_arrow->colour = glm::vec3(0.0, 0.0, 0.8);

    x_arrow->orientation = glm::vec3(0.0, glm::radians(90.0f), 0.0);
    y_arrow->orientation = glm::vec3(glm::radians(270.0f), 0.0, 0.0);
    z_arrow->orientation = glm::vec3(0.0, 0.0, 0.0);

    x_arrow->scale = glm::vec3(0.5, 0.5, 0.5);
    y_arrow->scale = glm::vec3(0.5, 0.5, 0.5);
    z_arrow->scale = glm::vec3(0.5, 0.5, 0.5);

    x_arrow->name = "X_AXIS_MOVE";
    y_arrow->name = "Y_AXIS_MOVE";
    z_arrow->name = "Z_AXIS_MOVE";

    // Set plane gizmo properties
    xy_plane_gizmo->colour = glm::vec3(0.8, 0.8, 0.0);
    xz_plane_gizmo->colour = glm::vec3(0.8, 0.0, 0.8);
    yz_plane_gizmo->colour = glm::vec3(0.0, 0.8, 0.8);

    xy_plane_gizmo->orientation = glm::vec3(0.0, 0.0, 0.0);
    xz_plane_gizmo->orientation = glm::vec3(glm::radians(90.0f), 0.0, 0.0);
    yz_plane_gizmo->orientation = glm::vec3(0.0, glm::radians(90.0f), 0.0);

    xy_plane_gizmo->scale = glm::vec3(0.15, 0.15, 0.005);
    xz_plane_gizmo->scale = glm::vec3(0.15, 0.15, 0.005);
    yz_plane_gizmo->scale = glm::vec3(0.15, 0.15, 0.005);

    xy_plane_gizmo->name = "XY_PLANE_MOVE";
    xz_plane_gizmo->name = "XZ_PLANE_MOVE";
    yz_plane_gizmo->name = "YZ_PLANE_MOVE";

    // Set the rotation gizmo properties
    x_rotation_gizmo->colour = glm::vec3(0.8, 0.0, 0.0);
    y_rotation_gizmo->colour = glm::vec3(0.0, 0.8, 0.0);
    z_rotation_gizmo->colour = glm::vec3(0.0, 0.0, 0.8);

    x_rotation_gizmo->orientation = glm::vec3(0.0, glm::radians(90.0f), 0.0);
    y_rotation_gizmo->orientation = glm::vec3(glm::radians(90.0f), 0.0, 0.0);
    z_rotation_gizmo->orientation = glm::vec3(0.0, 0.0, 0.0);

    x_rotation_gizmo->scale = glm::vec3(0.8, 0.8, 0.05);
    y_rotation_gizmo->scale = glm::vec3(0.9, 0.9, 0.05);
    z_rotation_gizmo->scale = glm::vec3(1.0, 1.0, 0.05);

    x_rotation_gizmo->name = "X_AXIS_ROTATION";
    y_rotation_gizmo->name = "Y_AXIS_ROTATION";
    z_rotation_gizmo->name = "Z_AXIS_ROTATION";

    addCube(glm::vec3(8.0, -2.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(0.0, 0.8, 0.0), 32.0);
    addCube(glm::vec3(-2.0, -2.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(0.0, 0.0, 0.6), 32.0);
    addPointLight(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2),
                  glm::vec3(1.0, 1.0, 1.0));
}

void Engine::update() {
    // Update the bounding box of the currently selected object
    if (selected_object) {
        selected_object->update_bounding_box();

        // Update the position and bounding boxes of the gizmos
        centre_gizmo->pos = selected_object->pos;

        x_arrow->pos = selected_object->pos + glm::vec3(0.25f * Arrow::tail_height, 0.0, 0.0);
        y_arrow->pos = selected_object->pos + glm::vec3(0.0, 0.25f * Arrow::tail_height, 0.0);
        z_arrow->pos = selected_object->pos + glm::vec3(0.0, 0.0, 0.25f * Arrow::tail_height);

        xy_plane_gizmo->pos = selected_object->pos + glm::vec3(0.25f * Arrow::tail_height,
                                                               0.25f * Arrow::tail_height, 0.0);
        xz_plane_gizmo->pos = selected_object->pos + glm::vec3(0.25f * Arrow::tail_height, 0.0,
                                                               0.25f * Arrow::tail_height);
        yz_plane_gizmo->pos = selected_object->pos + glm::vec3(0.0, 0.25f * Arrow::tail_height,
                                                               0.25f * Arrow::tail_height);

        x_rotation_gizmo->pos = selected_object->pos;
        y_rotation_gizmo->pos = selected_object->pos;
        z_rotation_gizmo->pos = selected_object->pos;

        x_arrow->update_bounding_box();
        y_arrow->update_bounding_box();
        z_arrow->update_bounding_box();

        xy_plane_gizmo->update_bounding_box();
        xz_plane_gizmo->update_bounding_box();
        yz_plane_gizmo->update_bounding_box();

        x_rotation_gizmo->update_bounding_box();
        y_rotation_gizmo->update_bounding_box();
        z_rotation_gizmo->update_bounding_box();
    }
    if (!mouse_pressed) {
        // If the mouse has been released, reset using_gizmo
        using_gizmo = false;
    }
}

void Engine::render() {

    // View and projection matrices won't change between objects
    glm::mat4 view = active_camera->lookAt();
    glm::mat4 projection =
        glm::perspective(glm::radians(active_camera->fov), (window_x / window_y), 0.1f, 100.f);

    gameobject_shader.use();
    gameobject_shader.setMat("view", view);
    gameobject_shader.setMat("projection", projection);
    updateShaderLights(gameobject_shader);
    gameobject_shader.setInt("point_lights_number", num_lights);

    light_shader.use();
    light_shader.setMat("view", view);
    light_shader.setMat("projection", projection);

    // Draw objects that don't need to be outlined first first

    for (int i = 0; i < game_objects.size(); ++i) {

        if (game_objects[i] == selected_object || game_objects[i] == mouseover_object) {
            continue;
        }

        // Draw lights one way, objects in another
        if (game_objects[i]->light) {
            game_objects[i]->draw(light_shader);
        } else {
            game_objects[i]->draw(gameobject_shader);
        }
    }

    // Draw objects that need to be outlined
    outline_shader.use();
    outline_shader.setMat("view", view);
    outline_shader.setMat("projection", projection);

    renderOutlinedObject();

    // Draw wireframe box representing the bounding box around objects that are
    // selected or mouseover'd
    if (mouseover_object) {
        renderBbox(mouseover_object, view, projection);
    }
    if (selected_object) {
        renderBbox(selected_object, view, projection);
        renderMoveGizmos(selected_object, view, projection);
    }
}

void Engine::render_imgui() {
    ImGui::Begin("ImGUI Window", NULL);

    ImGui::BeginTabBar("Settings#left_tabs_bar");
    // Object editor tab
    // -------------------------------------
    bool boolean = true;
    if (ImGui::BeginTabItem("Object Editor", &boolean, ImGuiTabItemFlags_None)) {
        ImGui::Text("Modify the selected objects properties");

        if (selected_object) {
            ImGui::NewLine();

            ImGui::Text("Position");

            ImGui::SetNextItemWidth(120.f);
            ImGui::InputFloat("x position", &selected_object->pos.x, 0.25f, 1.0f, "%.2f");
            ImGui::SetNextItemWidth(120.f);
            ImGui::InputFloat("y position", &selected_object->pos.y, 0.25f, 1.0f, "%.2f");
            ImGui::SetNextItemWidth(120.f);
            ImGui::InputFloat("z position", &selected_object->pos.z, 0.25f, 1.0f, "%.2f");

            ImGui::Separator();
            ImGui::Separator();

            ImGui::Text("Orientation");

            ImGui::SetNextItemWidth(120.f);
            ImGui::InputFloat("x orientation", &selected_object->orientation.x, 0.25f, 1.0f,
                              "%.2f");
            ImGui::SetNextItemWidth(120.f);
            ImGui::InputFloat("y orientation", &selected_object->orientation.y, 0.25f, 1.0f,
                              "%.2f");
            ImGui::SetNextItemWidth(120.f);
            ImGui::InputFloat("z orientation", &selected_object->orientation.z, 0.25f, 1.0f,
                              "%.2f");

            ImGui::Separator();
            ImGui::Separator();

            ImGui::Text("Scale");

            ImGui::SetNextItemWidth(120.f);
            ImGui::InputFloat("scale factor in x", &selected_object->scale.x, 0.25f, 1.0f, "%.2f");
            ImGui::SetNextItemWidth(120.f);
            ImGui::InputFloat("scale factor in y", &selected_object->scale.y, 0.25f, 1.0f, "%.2f");
            ImGui::SetNextItemWidth(120.f);
            ImGui::InputFloat("scale factor in z", &selected_object->scale.z, 0.25f, 1.0f, "%.2f");

            ImGui::Separator();
            ImGui::Separator();

            ImGui::ColorEdit3("Colour", glm::value_ptr(selected_object->colour));

            ImGui::SetNextItemWidth(120.f);
            ImGui::SliderFloat("Shininess", &selected_object->shininess, 0.0f, 100.0f);

            if (selected_object->light) {

                ImGui::NewLine();
                ImGui::NewLine();

                ImGui::Text("Modify the selected light's properties");

                ImGui::NewLine();

                ImGui::SetNextItemWidth(120.f);
                ImGui::SliderFloat("Ambient factor", &selected_object->light->ambient, 0, 1);
                ImGui::SetNextItemWidth(120.f);
                ImGui::SliderFloat("Diffuse factor", &selected_object->light->diffuse, 0, 1);
                ImGui::SetNextItemWidth(120.f);
                ImGui::SliderFloat("Specular factor", &selected_object->light->specular, 0, 1);

                ImGui::Separator();
                ImGui::Separator();

                ImGui::SetNextItemWidth(120.f);
                ImGui::SliderFloat("Constant attenuation factor", &selected_object->light->constant,
                                   0, 4);
                ImGui::SetNextItemWidth(120.f);
                ImGui::SliderFloat("Linear attenuation factor", &selected_object->light->linear, 0,
                                   0.5);
                ImGui::SetNextItemWidth(120.f);
                ImGui::SliderFloat("Quadratic attenuation factor",
                                   &selected_object->light->quadratic, 0, 0.1);
            }

            /*
            char temp_name[255] = "";
            ImGui::InputTextWithHint("Object Name", selected_object->name.c_str(), temp_name,
            IM_ARRAYSIZE(temp_name)); ImGui::LabelText(selected_object->name.c_str(), "Game
            Object"); selected_object->name = temp_name;
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
                selected_object  = *it;
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
            } else {
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

        ImGui::NewLine();

        ImGui::Text("Position");

        ImGui::SetNextItemWidth(120.f);
        ImGui::InputFloat("x position", &placeholder_pos.x, 0.25f, 1.0f, "%.2f");
        ImGui::SetNextItemWidth(120.f);
        ImGui::InputFloat("y position", &placeholder_pos.y, 0.25f, 1.0f, "%.2f");
        ImGui::SetNextItemWidth(120.f);
        ImGui::InputFloat("z position", &placeholder_pos.z, 0.25f, 1.0f, "%.2f");

        ImGui::Separator();
        ImGui::Separator();

        ImGui::Text("Orientation");

        ImGui::SetNextItemWidth(120.f);
        ImGui::InputFloat("x orientation", &placeholder_orientation.x, 0.25f, 1.0f, "%.2f");
        ImGui::SetNextItemWidth(120.f);
        ImGui::InputFloat("y orientation", &placeholder_orientation.y, 0.25f, 1.0f, "%.2f");
        ImGui::SetNextItemWidth(120.f);
        ImGui::InputFloat("z orientation", &placeholder_orientation.z, 0.25f, 1.0f, "%.2f");

        ImGui::Separator();
        ImGui::Separator();

        ImGui::Text("Scale");

        ImGui::SetNextItemWidth(120.f);
        ImGui::InputFloat("scale factor in x", &placeholder_scale.x, 0.25f, 1.0f, "%.2f");
        ImGui::SetNextItemWidth(120.f);
        ImGui::InputFloat("scale factor in y", &placeholder_scale.y, 0.25f, 1.0f, "%.2f");
        ImGui::SetNextItemWidth(120.f);
        ImGui::InputFloat("scale factor in z", &placeholder_scale.z, 0.25f, 1.0f, "%.2f");

        ImGui::Separator();
        ImGui::Separator();

        ImGui::ColorEdit3("Colour", glm::value_ptr(placeholder_colour));

        ImGui::SetNextItemWidth(120.f);
        ImGui::SliderFloat("Shininess", &placeholder_shininess, 0.0f, 100.0f);

        if (ImGui::Button("Add Object")) {
            addCube(placeholder_pos, placeholder_orientation, placeholder_scale, placeholder_colour,
                    placeholder_shininess);
            selected_object  = game_objects[game_objects.size() - 1];
            mouseover_object = nullptr;
        }

        ImGui::NewLine();
        ImGui::NewLine();

        ImGui::Text("Modify the light properties");

        ImGui::NewLine();

        ImGui::SetNextItemWidth(120.f);
        ImGui::SliderFloat("Ambient factor", &placeholder_light.ambient, 0, 1);
        ImGui::SetNextItemWidth(120.f);
        ImGui::SliderFloat("Diffuse factor", &placeholder_light.diffuse, 0, 1);
        ImGui::SetNextItemWidth(120.f);
        ImGui::SliderFloat("Specular factor", &placeholder_light.specular, 0, 1);

        ImGui::Separator();
        ImGui::Separator();

        ImGui::SetNextItemWidth(120.f);
        ImGui::SliderFloat("Constant attenuation factor", &placeholder_light.constant, 0, 4);
        ImGui::SetNextItemWidth(120.f);
        ImGui::SliderFloat("Linear attenuation factor", &placeholder_light.linear, 0, 0.5);
        ImGui::SetNextItemWidth(120.f);
        ImGui::SliderFloat("Quadratic attenuation factor", &placeholder_light.quadratic, 0, 0.1);

        if (ImGui::Button("Add Light Object")) {
            if (num_lights >= max_lights) {
                std::cout << "Max number of lights reached" << std::endl;
            } else {
                unsigned int n = game_objects.size();
                addCube(placeholder_pos, placeholder_orientation, placeholder_scale,
                        placeholder_colour, placeholder_shininess);

                game_objects[n]->add_light(placeholder_light.ambient, placeholder_light.diffuse,
                                           placeholder_light.specular, placeholder_light.constant,
                                           placeholder_light.linear, placeholder_light.quadratic);

                num_lights++;

                selected_object  = game_objects[n];
                mouseover_object = nullptr;
            }
        }

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::End();
}

void Engine::processMouseMovement(float mouse_x, float mouse_y) {
    // Check which regular GameObjects the mouse is hovering over
    mouseObjectsIntersect(mouse_x, mouse_y);

    // Check which gizmos the mouse is hovering over if an object is currently not
    // selected, no gizmo visible, don't bother with checks. Avoid checks if mouse
    // is being pressed
    if (selected_object && !mouse_pressed) {
        mouseGizmosIntersect(mouse_x, mouse_y);
    }

    // Mouse is over gizmo and mouse button is being pressed. Process gizmo function
    if (active_gizmo && mouse_pressed) {
        glm::vec3 mouse_ray = mouseRaycast(mouse_x, mouse_y);

        std::string name = active_gizmo->name;

        if (name == "X_AXIS_MOVE" || name == "Y_AXIS_MOVE" || name == "Z_AXIS_MOVE") {
            gizmoAxisMoveFunction(mouse_ray, active_gizmo);
        } else if (name == "XY_PLANE_MOVE" || name == "XZ_PLANE_MOVE" || name == "YZ_PLANE_MOVE") {
            gizmoPlaneFunction(mouse_ray, active_gizmo);
        } else {
            gizmoRotationFunction(mouse_ray, active_gizmo);
        }
    }
}

void Engine::processMouseClick() {
    if (mouseover_object) {
        selected_object  = mouseover_object;
        mouseover_object = NULL;
    }
}

void Engine::setActiveGizmoType(enum GizmoType gizmo_type) {
    active_gizmo_type = gizmo_type;
}

void Engine::setWindowSize(float window_x, float window_y) {
    this->window_x = window_x;
    this->window_y = window_y;
}

void Engine::resetObjectPointers() {
    this->mouseover_object = nullptr;
    this->selected_object  = nullptr;
}

// Private functions
// --------------------------------------------------------------------------------------

void Engine::addCube(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour,
                     float shininess) {
    // Use this number to name the object (cube)
    unsigned int n = game_objects.size();

    game_objects.push_back(std::make_shared<Cube>(pos, orientation, scale, colour, shininess));

    game_objects[n]->name = "Object_" + std::to_string(n);
}

void Engine::addPointLight(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale,
                           glm::vec3 colour) {
    unsigned int n = game_objects.size();
    addCube(pos, orientation, scale, colour, 32.0f);

    game_objects[n]->add_light(0.1, 0.8, 1.0, 1.0, 0.09, 0.032);
    game_objects[n]->name = "Object_" + std::to_string(n);

    num_lights++;
}

void Engine::updateShaderLights(Shader& shader) {
    shader.use();
    unsigned int counter = 0;

    for (unsigned int i = 0; i < game_objects.size(); ++i) {

        if (counter > max_lights) {
            std::cout << "Counter somehow greater than number of max light" << std::endl;
        }

        if (game_objects[i]->light) {
            shader.setVec3("point_lights[" + std::to_string(counter) + "].position",
                           game_objects[i]->pos);
            shader.setVec3("point_lights[" + std::to_string(counter) + "].colour",
                           game_objects[i]->colour);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].ambient",
                            game_objects[i]->light->ambient);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].diffuse",
                            game_objects[i]->light->diffuse);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].specular",
                            game_objects[i]->light->specular);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].constant",
                            game_objects[i]->light->constant);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].linear",
                            game_objects[i]->light->linear);
            shader.setFloat("point_lights[" + std::to_string(counter) + "].quadratic",
                            game_objects[i]->light->quadratic);

            counter++;
        }
    }
}

void Engine::mouseObjectsIntersect(float mouse_x, float mouse_y) {
    glm::vec3 mouse_direction = mouseRaycast(mouse_x, mouse_y);

    // Keep track of whether any object is under the mouse
    bool mouseover = false;

    for (unsigned int i = 0; i < game_objects.size(); ++i) {
        if (mouseIntersectsBoundingBox(mouse_direction, game_objects[i]->bbox)) {
            if (selected_object == game_objects[i] && selected_object) {
                // Ignore this check if the item currently being hovered over is the
                // selected object. Ensure selected_object is an actual object and not
                // just NULL
                continue;
            }
            mouseover        = true;
            mouseover_object = game_objects[i];
        }
    }
    if (!mouseover) {
        // If no object has mouse over it, set mouseover_object to NULL
        mouseover_object = nullptr;
    }
}

void Engine::mouseGizmosIntersect(float mouse_x, float mouse_y) {
    // Check if the mouse is hovering over the positions arrows
    glm::vec3 mouse_direction = mouseRaycast(mouse_x, mouse_y);

    bool mouse_over_gizmo = false;

    // Only check for the active gizmo type
    if (active_gizmo_type == MOVE) {
        if (mouseIntersectsBoundingBox(mouse_direction, x_arrow->bbox)) {
            active_gizmo     = x_arrow;
            mouse_over_gizmo = true;
        }
        if (mouseIntersectsBoundingBox(mouse_direction, y_arrow->bbox)) {
            active_gizmo     = y_arrow;
            mouse_over_gizmo = true;
        }
        if (mouseIntersectsBoundingBox(mouse_direction, z_arrow->bbox)) {
            active_gizmo     = z_arrow;
            mouse_over_gizmo = true;
        }
        if (mouseIntersectsBoundingBox(mouse_direction, xy_plane_gizmo->bbox)) {
            active_gizmo     = xy_plane_gizmo;
            mouse_over_gizmo = true;
        }
        if (mouseIntersectsBoundingBox(mouse_direction, xz_plane_gizmo->bbox)) {
            active_gizmo     = xz_plane_gizmo;
            mouse_over_gizmo = true;
        }
        if (mouseIntersectsBoundingBox(mouse_direction, yz_plane_gizmo->bbox)) {
            active_gizmo     = yz_plane_gizmo;
            mouse_over_gizmo = true;
        }
    } else if (active_gizmo_type == ROTATE) {
        if (mouseIntersectsBoundingBox(mouse_direction, x_rotation_gizmo->bbox)) {
            active_gizmo     = x_rotation_gizmo;
            mouse_over_gizmo = true;
        }
        if (mouseIntersectsBoundingBox(mouse_direction, y_rotation_gizmo->bbox)) {
            active_gizmo     = y_rotation_gizmo;
            mouse_over_gizmo = true;
        }
        if (mouseIntersectsBoundingBox(mouse_direction, z_rotation_gizmo->bbox)) {
            active_gizmo     = z_rotation_gizmo;
            mouse_over_gizmo = true;
        }
    }

    if (!mouse_over_gizmo) {
        active_gizmo = nullptr;
    }
}

glm::vec3 Engine::mouseRaycast(float mouse_x, float mouse_y) {
    // Mouse position in x and y with it's z at the very far end of the
    // NDC space
    glm::vec3 mouse_ndc((2.0f * mouse_x - window_x) / window_x,
                        (window_y - 2.0f * mouse_y) / window_y, 1.0f);

    // Mouse in clip space with x and y, z is set at just one unit in front of the camera
    // w coordinate set to 1 to keep things simple
    glm::vec4 mouse_clip = glm::vec4(mouse_ndc.x, mouse_ndc.y, 1.0, 1.0);

    glm::mat4 projection =
        glm::perspective(glm::radians(active_camera->fov), (window_x / window_y), 0.1f, 100.f);

    glm::mat4 view = active_camera->lookAt();

    // Find position of the mouse in world space coordinates using the inverse
    // transformation of clip space -> world space
    glm::vec4 mouse_world = glm::inverse(projection * view) * mouse_clip;
    mouse_world /= mouse_world.w;

    // Find the direction of the vector pointing FROM the camera TO the mouse
    glm::vec3 mouse_ray_direction = glm::normalize(glm::vec3(mouse_world) - active_camera->pos);

    return mouse_ray_direction;
}

bool Engine::mouseIntersectsBoundingBox(glm::vec3 mouse_direction, AABB bbox) {
    glm::vec3 dirfrac;
    dirfrac.x = 1.0f / mouse_direction.x;
    dirfrac.y = 1.0f / mouse_direction.y;
    dirfrac.z = 1.0f / mouse_direction.z;

    float t1 = (bbox.xmin - active_camera->pos.x) * dirfrac.x;
    float t2 = (bbox.xmax - active_camera->pos.x) * dirfrac.x;
    float t3 = (bbox.ymin - active_camera->pos.y) * dirfrac.y;
    float t4 = (bbox.ymax - active_camera->pos.y) * dirfrac.y;
    float t5 = (bbox.zmin - active_camera->pos.z) * dirfrac.z;
    float t6 = (bbox.zmax - active_camera->pos.z) * dirfrac.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0) {
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax) {
        return false;
    }

    return true;
}

bool Engine::rayPlaneIntersection(glm::vec3& ray_origin, glm::vec3& ray_direction,
                                  glm::vec3& plane_normal, glm::vec3& plane_position, float& t) {
    // Ray direction and Plane Normal MUST be normalised
    float denom = glm::dot(plane_normal, ray_direction);
    if (std::abs(denom) > 1e-6) {
        glm::vec3 p0_l0 = plane_position - ray_origin;
        t               = glm::dot(p0_l0, plane_normal) / denom;
        return (t >= 0);
    }
    return false;
}

glm::vec3 Engine::closestPointBetweenRays(glm::vec3& ray1_origin, glm::vec3& ray1_direction,
                                          glm::vec3& ray2_origin, glm::vec3& ray2_direction) {
    // Returns the closest point between ray1 and ray2.
    // Closest point lies on ray1.
    // Directions MUST be normalised

    glm::vec3 s_d = ray1_origin - ray2_origin;

    float dadb  = glm::dot(ray1_direction, ray2_direction);
    float dasd  = glm::dot(ray1_direction, s_d);
    float dbsd  = glm::dot(ray2_direction, s_d);
    float denom = 1.0f - dadb * dadb;

    if (denom == 0.0) {
        std::cout << "rays are parallel" << std::endl;
        return glm::vec3(0.0, 0.0, 0.0);
    }

    return ((-dasd + dadb * dbsd) / denom) * ray1_direction + ray1_origin;
}

void Engine::gizmoAxisMoveFunction(glm::vec3& mouse_ray, std::shared_ptr<GameObject> gizmo) {
    glm::vec3 axis(0.0, 0.0, 0.0);

    if (gizmo->name == "X_AXIS_MOVE") {
        axis = glm::vec3(1.0, 0.0, 0.0);
    } else if (gizmo->name == "Y_AXIS_MOVE") {
        axis = glm::vec3(0.0, 1.0, 0.0);
    } else if (gizmo->name == "Z_AXIS_MOVE") {
        axis = glm::vec3(0.0, 0.0, 1.0);
    }

    glm::vec3 closest_point_on_axis =
        closestPointBetweenRays(gizmo->pos, axis, active_camera->pos, mouse_ray);

    if (!using_gizmo) {
        previous_position = closest_point_on_axis;
        using_gizmo       = true;
    } else {
        selected_object->pos += (closest_point_on_axis - previous_position);
        previous_position = closest_point_on_axis;
    }
}

void Engine::gizmoPlaneFunction(glm::vec3& mouse_ray, std::shared_ptr<GameObject> gizmo) {
    float t = 0.0; // Ray from camera to mouse parameter

    glm::vec3 axis1(0.0, 0.0, 0.0);
    glm::vec3 axis2(0.0, 0.0, 0.0);

    if (gizmo->name == "XY_PLANE_MOVE") {
        axis1 = glm::vec3(1.0, 0.0, 0.0);
        axis2 = glm::vec3(0.0, 1.0, 0.0);
    } else if (gizmo->name == "XZ_PLANE_MOVE") {
        axis1 = glm::vec3(1.0, 0.0, 0.0);
        axis2 = glm::vec3(0.0, 0.0, 1.0);
    } else if (gizmo->name == "YZ_PLANE_MOVE") {
        axis1 = glm::vec3(0.0, 1.0, 0.0);
        axis2 = glm::vec3(0.0, 0.0, 1.0);
    }

    glm::vec3 plane_normal = glm::normalize(glm::cross(axis1, axis2));

    if (!rayPlaneIntersection(active_camera->pos, mouse_ray, plane_normal, gizmo->pos, t)) {
        return;
    }

    glm::vec3 plane_intersection = active_camera->pos + mouse_ray * t;

    if (!using_gizmo) {
        previous_position = plane_intersection;
        using_gizmo       = true;
    } else {
        selected_object->pos += (plane_intersection - previous_position);
        previous_position = plane_intersection;
    }
}

void Engine::gizmoRotationFunction(glm::vec3& mouse_ray, std::shared_ptr<GameObject> gizmo) {
    float t = 0.0; // Ray from camera to mouse parameter

    glm::vec3 axis1(0.0, 0.0, 0.0);
    glm::vec3 axis2(0.0, 0.0, 0.0);

    glm::vec3 rotation_axis(0.0, 0.0, 0.0);

    if (gizmo->name == "X_AXIS_ROTATION") {
        axis1         = glm::vec3(0.0, 1.0, 0.0);
        axis2         = glm::vec3(0.0, 0.0, 1.0);
        rotation_axis = glm::vec3(1.0, 0.0, 0.0);
    } else if (gizmo->name == "Y_AXIS_ROTATION") {
        axis1         = glm::vec3(1.0, 0.0, 0.0);
        axis2         = glm::vec3(0.0, 0.0, 1.0);
        rotation_axis = glm::vec3(0.0, 1.0, 0.0);
    } else if (gizmo->name == "Z_AXIS_ROTATION") {
        axis1         = glm::vec3(1.0, 0.0, 0.0);
        axis2         = glm::vec3(0.0, 1.0, 0.0);
        rotation_axis = glm::vec3(0.0, 0.0, 1.0);
    }

    glm::vec3 plane_normal = glm::normalize(glm::cross(axis1, axis2));

    if (!rayPlaneIntersection(active_camera->pos, mouse_ray, plane_normal, gizmo->pos, t)) {
        return;
    }

    glm::vec3 plane_intersection = active_camera->pos + mouse_ray * t;

    if (!using_gizmo) {
        previous_position = plane_intersection;
        using_gizmo       = true;
        return;
    }

    glm::vec3 a   = previous_position - selected_object->pos;
    glm::vec3 b   = plane_intersection - selected_object->pos;
    float det_abu = glm::dot(glm::cross(a, b), rotation_axis);

    float angle = std::acos(glm::dot(a, b) / (glm::length(a) * glm::length(b)));

    if (det_abu < 0) {
        angle *= -1;
    }

    selected_object->orientation += angle * rotation_axis;

    previous_position = plane_intersection;
}

void Engine::renderOutlinedObject() {

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // Draw selected object
    if (selected_object) {

        if (selected_object->light) {
            selected_object->draw(light_shader);
        } else {
            selected_object->draw(gameobject_shader);
        }
        // Draw outline of selected object
        selected_object->scale *= 1.05;
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);

        glDisable(GL_DEPTH_TEST);

        selected_object->draw(outline_shader);

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
        glStencilMask(0xFF);
        selected_object->scale /= 1.05;
    }

    // Draw mouseover object
    if (mouseover_object) {
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        if (mouseover_object->light) {
            mouseover_object->draw(light_shader);
        } else {
            mouseover_object->draw(gameobject_shader);
        }

        // Draw outline of mouseover object
        mouseover_object->scale *= 1.05;
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);

        glDisable(GL_DEPTH_TEST);

        mouseover_object->draw(outline_shader);

        // Reset mouseover object size and stencil, depth buffer settings
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
        glStencilMask(0xFF);
        mouseover_object->scale /= 1.05;
    }

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void Engine::renderBbox(std::shared_ptr<GameObject> game_object, glm::mat4& view,
                        glm::mat4& projection) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    bbox_wireframe.pos    = game_object->pos;
    bbox_wireframe.colour = game_object->colour;

    bbox_wireframe.scale.x = game_object->bbox.xmax - game_object->bbox.xmin;
    bbox_wireframe.scale.y = game_object->bbox.ymax - game_object->bbox.ymin;
    bbox_wireframe.scale.z = game_object->bbox.zmax - game_object->bbox.zmin;

    // Going to use a simple fragment shader for the wireframe box
    light_shader.use();
    light_shader.setMat("projection", projection);
    light_shader.setMat("view", view);
    bbox_wireframe.draw(light_shader);

    // Set polygon mode back to normal after rendering the wireframe box
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Engine::renderMoveGizmos(std::shared_ptr<GameObject> game_object, glm::mat4& view,
                              glm::mat4& projection) {
    glDisable(GL_DEPTH_TEST);

    // Make active gizmo invisble until it is drawn larger
    if (active_gizmo) {
        active_gizmo->visible = false;
    }

    light_shader.use();
    light_shader.setMat("projection", projection);
    light_shader.setMat("view", view);

    if (active_gizmo_type == MOVE) {
        x_arrow->draw(light_shader);
        y_arrow->draw(light_shader);
        z_arrow->draw(light_shader);

        xy_plane_gizmo->draw(light_shader);
        xz_plane_gizmo->draw(light_shader);
        yz_plane_gizmo->draw(light_shader);
    }

    // Render the active gizmo but larger
    if (active_gizmo && active_gizmo_type == MOVE) {
        // Make active gizmo visible again
        active_gizmo->visible = true;

        active_gizmo->scale *= 1.2f;
        active_gizmo->colour *= 1.2f;

        active_gizmo->draw(light_shader);

        active_gizmo->scale /= 1.2f;
        active_gizmo->colour /= 1.2f;
    }

    // Want the centre gizmo to always be visible so draw it last
    centre_gizmo->draw(light_shader);

    glEnable(GL_DEPTH_TEST);

    // Rendering the rotation gizmos here with GL_DEPTH_TEST enabled because it looks
    // better
    if (active_gizmo_type == ROTATE) {
        x_rotation_gizmo->draw(light_shader);
        y_rotation_gizmo->draw(light_shader);
        z_rotation_gizmo->draw(light_shader);
    }

    if (active_gizmo && active_gizmo_type == ROTATE) {
        // Make active gizmo visible again
        active_gizmo->visible = true;

        active_gizmo->scale *= 1.05f;
        active_gizmo->colour *= 1.2f;

        active_gizmo->draw(light_shader);

        active_gizmo->scale /= 1.05f;
        active_gizmo->colour /= 1.2f;
    }
}
