#include <app.hpp>

App::App(int window_x, int window_y)
    : renderer(window_x, window_y, 16)
    , max_lights(16) {
    // Window properties
    this->window_x = window_x;
    this->window_y = window_y;
    event_manager  = std::make_shared<EventManager>();
    window_manager = makeGLFWWindowManager(window_x, window_y, event_manager);

    // Timings
    delta_time = 0.0f;
    last_frame = 0.0f;

    // Mouse
    first_mouse_movement = true;
    last_mouse_xpos      = window_x / 2.f;
    last_mouse_ypos      = window_y / 2.f;
    mouse_pressed        = false;

    // Camera
    active_camera = &engine_camera;

    // Shaders
    num_lights = 0;

    // Objects
    mouseover_object = nullptr;
    selected_object  = nullptr;

    // Data for placeholder object
    placeholder_pos         = glm::vec3(0.0, 0.0, 0.0);
    placeholder_orientation = glm::vec3(0.0, 0.0, 0.0);
    placeholder_scale       = glm::vec3(1.0, 1.0, 1.0);
    placeholder_colour      = glm::vec3(0.0, 0.0, 0.0);
    placeholder_shininess   = 85.0f;
    placeholder_light       = Light(0.1, 0.8, 1.0, 1.0, 0.09, 0.032);

    placeholder_object_type = "CUBE";
    object_type_list        = {"CUBE", "SPHERE", "ARROW", "HOLLOW_CYLINDER"};

    // Gizmos
    mouseover_gizmo = nullptr;
    centre_gizmo    = std::make_shared<Sphere>();

    // Gizmo functionality
    using_gizmo       = false;
    previous_position = glm::vec3(0.0, 0.0, 0.0);
    active_gizmo_type = GizmoType::MOVE;

    draw_normals = false;
}

App::~App() {}

void App::run() {
    if (!init()) {
        // If initialisation failed somehow, quit app
        std::cout << "Initialisation failed" << std::endl;
        return;
    }

    while (!window_manager->shouldWindowClose()) {
        update();

        render();
    }
}

void App::resetObjectPointers() {
    this->mouseover_object = nullptr;
    this->selected_object  = nullptr;
}

bool App::init() {

    // Create window with the window manager. Window manager responsible for initialising
    // GLFW, OpenGL and ImGUI
    if (!window_manager->createWindow()) {
        std::cout << "Window manager failed to initialise" << std::endl;
        return false;
    }

    // Initialise renderer
    renderer.init();

    // Initialise object static variables
    initObjects();

    // Initialise gizmo properties
    initGizmos();

    // Initialise the scene
    initScene();

    return true;
}

void App::initObjects() {
    // Initialise Cube static variables that need OpenGL to exist
    Cube::init();
    // Initialise Arrow static variables
    Arrow::init();
    // Initialise sphere
    Sphere::init();
    // Initialise hollow cylinder
    HollowCylinder::init();
}

void App::initGizmos() {
    gizmos["X_AXIS_MOVE"] = std::make_shared<AxisMoveGizmo>("X");
    gizmos["X_AXIS_MOVE"]->toggleActivity();

    gizmos["Y_AXIS_MOVE"] = std::make_shared<AxisMoveGizmo>("Y");
    gizmos["Y_AXIS_MOVE"]->toggleActivity();

    gizmos["Z_AXIS_MOVE"] = std::make_shared<AxisMoveGizmo>("Z");
    gizmos["Z_AXIS_MOVE"]->toggleActivity();

    gizmos["XY_PLANE_MOVE"] = std::make_shared<PlaneMoveGizmo>("XY");
    gizmos["XY_PLANE_MOVE"]->toggleActivity();

    gizmos["XZ_PLANE_MOVE"] = std::make_shared<PlaneMoveGizmo>("XZ");
    gizmos["XZ_PLANE_MOVE"]->toggleActivity();

    gizmos["YZ_PLANE_MOVE"] = std::make_shared<PlaneMoveGizmo>("YZ");
    gizmos["YZ_PLANE_MOVE"]->toggleActivity();

    gizmos["X_AXIS_ROTATE"] = std::make_shared<RotateGizmo>("X");
    gizmos["Y_AXIS_ROTATE"] = std::make_shared<RotateGizmo>("Y");
    gizmos["Z_AXIS_ROTATE"] = std::make_shared<RotateGizmo>("Z");

    // Set gizmo properties
    centre_gizmo->colour = glm::vec3(0.8, 0.8, 0.8);
    centre_gizmo->scale  = glm::vec3(0.05, 0.05, 0.05);
    centre_gizmo->name   = "CENTRE_GIZMO";
}

void App::initScene() {
    addCube(glm::vec3(8.0, -2.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(0.0, 0.8, 0.0), 85.0);
    addCube(glm::vec3(-2.0, -2.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0),
            glm::vec3(0.0, 0.0, 0.6), 85.0);
    addPointLight(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.2, 0.2, 0.2),
                  glm::vec3(1.0, 1.0, 1.0));
}

void App::update() {
    float current_frame = window_manager->getTime();
    delta_time          = current_frame - last_frame;
    last_frame          = current_frame;

    // Update the bounding box of the currently selected object
    if (selected_object) {
        selected_object->update_bounding_box();

        // Update the position and bounding boxes of the gizmos
        centre_gizmo->pos = selected_object->pos;

        // Update the position of the gizmos
        for (auto& it : gizmos) {
            it.second->updatePosAndOrientation(selected_object);
            it.second->updateBoundingBox();
        }
    }

    runActions();

    window_manager->update();

    event_manager->update();
}

void App::runActions() {
    for (; !event_manager->events.empty(); event_manager->events.pop()) {
        // Consider using a switch statement for the below
        switch (event_manager->events.front().action) {
        case Action::SCREEN_RESIZE:
            processScreenResize(event_manager->events.front().mouse_xpos,
                                event_manager->events.front().mouse_ypos);
            break;
        case Action::MOVE_FORWARD:
            active_camera->processKeyboard(FORWARD, delta_time);
            break;
        case Action::MOVE_LEFT:
            active_camera->processKeyboard(LEFT, delta_time);
            break;
        case Action::MOVE_BACKWARD:
            active_camera->processKeyboard(BACKWARD, delta_time);
            break;
        case Action::MOVE_RIGHT:
            active_camera->processKeyboard(RIGHT, delta_time);
            break;
        case Action::MOVE_UP:
            active_camera->processKeyboard(UP, delta_time);
            break;
        case Action::MOVE_DOWN:
            active_camera->processKeyboard(DOWN, delta_time);
            break;
        case Action::TURN: {
            float mouse_xoffset = event_manager->events.front().mouse_xpos - last_mouse_xpos;
            float mouse_yoffset = event_manager->events.front().mouse_ypos - last_mouse_ypos;
            last_mouse_xpos += mouse_xoffset;
            last_mouse_ypos += mouse_yoffset;

            if (!first_mouse_movement) {
                active_camera->processMouse(mouse_xoffset, mouse_yoffset);
            } else {
                first_mouse_movement = false;
            }
            break;
        }
        case Action::MOVE_CURSOR: {
            float mouse_xoffset = event_manager->events.front().mouse_xpos - last_mouse_xpos;
            float mouse_yoffset = event_manager->events.front().mouse_ypos - last_mouse_ypos;
            last_mouse_xpos += mouse_xoffset;
            last_mouse_ypos += mouse_yoffset;
            processMouseMovement(last_mouse_xpos, last_mouse_ypos);
            break;
        }
        case Action::ZOOM:
            active_camera->processScroll(event_manager->events.front().scroll_x,
                                         event_manager->events.front().scroll_y);
            break;
        case Action::L_CLICK:
            // Once a click has been registered, i.e. left mouse button has been released:
            using_gizmo   = false;
            mouse_pressed = false;
            if (mouseover_object) {
                selected_object  = mouseover_object;
                mouseover_object = nullptr;
            }
            break;
        case Action::L_BUTTON_PRESSED:
            mouse_pressed = true;
            break;
        case Action::R_CLICK:
            std::cout << "Right click registered" << std::endl;
            break;
        case Action::R_BUTTON_PRESSED:
            std::cout << "Right button being held" << std::endl;
            break;
        case Action::TOGGLE_MOUSE:
            window_manager->toggleMouse();
            first_mouse_movement = true;
            break;
        case Action::TOGGLE_GIZMO:
            if (active_gizmo_type == GizmoType::MOVE) {
                active_gizmo_type = GizmoType::ROTATE;
            } else {
                active_gizmo_type = GizmoType::MOVE;
            }
            for (auto& it : gizmos) {
                it.second->toggleActivity();
            }
            break;
        case Action::SAVE_SCENE:
            SceneSaver::saveScene(*this);
            break;
        case Action::LOAD_SCENE:
            SceneSaver::loadScene(*this);
            break;
        }
    }
}

void App::render() {
    renderer.renderPrep(active_camera);

    // After clearing the OpenGL buffer, need to let ImGui know that we are now going
    // to work on the new frame
    window_manager->newImGuiFrame();

    // Disable the objects that need to be outlined for scene rendering
    if (selected_object) {
        selected_object->visible = false;
    }
    if (mouseover_object) {
        mouseover_object->visible = false;
    }

    // Render scene
    renderer.renderScene(game_objects, active_camera);

    // Make objects that need to be outlined visible again
    if (selected_object) {
        selected_object->visible = true;
    }
    if (mouseover_object) {
        mouseover_object->visible = true;
    }
    renderer.renderOutlinedObjects(selected_object, mouseover_object);

    if (draw_normals) {
        renderer.renderNormals(game_objects);
    }

    // Draw wireframe box representing the bounding box around objects that are
    // selected or mouseover'd
    if (mouseover_object) {
        renderer.renderBbox(mouseover_object, bbox_wireframe);
    }
    if (selected_object) {
        renderer.renderBbox(selected_object, bbox_wireframe);
        renderer.renderGizmos(gizmos, mouseover_gizmo, active_gizmo_type);
    }

    renderer.renderScreen();

    // After drawing OpenGL objects, draw ImGUI
    renderImGUI();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Final step, render to window
    window_manager->renderToWindow();
}

void App::renderImGUI() {
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(window_x * 0.15f, window_y));
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
            ImGui::SliderFloat("Shininess", &selected_object->shininess, 0.0f, 200.0f);

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
                mouseover_object = nullptr;
            }
            ImGui::SameLine();
            std::string button_id = "Delete Object##";
            button_id += object_name;
            if (ImGui::Button(button_id.c_str())) {
                if (selected_object == *it) {
                    selected_object = nullptr;
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

        ImGui::Text("Type of object");
        if (ImGui::BeginCombo("Select type", placeholder_object_type.c_str())) {
            for (unsigned int i = 0; i < object_type_list.size(); ++i) {
                bool is_selected = (placeholder_object_type == object_type_list[i]);
                if (ImGui::Selectable(object_type_list[i].c_str(), is_selected)) {
                    placeholder_object_type = object_type_list[i];
                }
            }
            ImGui::EndCombo();
        }
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
        ImGui::SliderFloat("Shininess", &placeholder_shininess, 0.0f, 200.0f);

        if (ImGui::Button("Add Object")) {
            addPlaceholderObject();
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
                addPlaceholderObject();

                game_objects[n]->add_light(placeholder_light.ambient, placeholder_light.diffuse,
                                           placeholder_light.specular, placeholder_light.constant,
                                           placeholder_light.linear, placeholder_light.quadratic);

                num_lights++;
                renderer.setNumLights(num_lights);

                selected_object  = game_objects[n];
                mouseover_object = nullptr;
            }
        }

        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(window_x - (window_x * 0.15f), 0.0f));
    ImGui::SetNextWindowSize(ImVec2(window_x * 0.15f, window_y * 0.3f));
    ImGui::Begin("Skybox Menu");

    ImGui::SetNextItemWidth(120.f);
    if (ImGui::BeginCombo("Select skybox", renderer.get_active_skybox_name().c_str())) {
        for (auto it = renderer.get_skyboxes().begin(); it != renderer.get_skyboxes().end(); ++it) {
            bool is_selected = (renderer.get_active_skybox_name() == it->first);
            if (ImGui::Selectable(it->first.c_str(), is_selected)) {
                renderer.get_active_skybox_name() = it->first;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();
    ImGui::Separator();
    ImGui::SetNextItemWidth(120.f);
    ImGui::SliderFloat("Skybox brightness", &Skybox::brightness, 0.0, 1.0);

    ImGui::Separator();
    ImGui::Separator();
    ImGui::Text("Toggle Normal Visualisation");
    ImGui::Checkbox("Visualise Normals", &draw_normals);

    ImGui::End();
}

void App::addCube(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour,
                  float shininess) {
    // Use this number to name the object (cube)
    unsigned int n = game_objects.size();

    game_objects.push_back(std::make_shared<Cube>(pos, orientation, scale, colour, shininess));

    game_objects[n]->name = "Object_" + std::to_string(n);
}

void App::addPlaceholderObject() {
    unsigned int n = game_objects.size();
    if (this->placeholder_object_type == "CUBE") {
        game_objects.push_back(std::make_shared<Cube>(placeholder_pos, placeholder_orientation,
                                                      placeholder_scale, placeholder_colour,
                                                      placeholder_shininess));
    } else if (this->placeholder_object_type == "SPHERE") {
        game_objects.push_back(std::make_shared<Sphere>(placeholder_pos, placeholder_orientation,
                                                        placeholder_scale, placeholder_colour,
                                                        placeholder_shininess));
    } else if (this->placeholder_object_type == "ARROW") {
        game_objects.push_back(std::make_shared<Arrow>(placeholder_pos, placeholder_orientation,
                                                       placeholder_scale, placeholder_colour,
                                                       placeholder_shininess));
    } else if (this->placeholder_object_type == "HOLLOW_CYLINDER") {
        game_objects.push_back(std::make_shared<HollowCylinder>(
            placeholder_pos, placeholder_orientation, placeholder_scale, placeholder_colour,
            placeholder_shininess));
    }
    game_objects[n]->name = "Object_" + std::to_string(n);
}

void App::addPointLight(glm::vec3 pos, glm::vec3 orientation, glm::vec3 scale, glm::vec3 colour) {
    unsigned int n = game_objects.size();
    addCube(pos, orientation, scale, colour, 85.0f);

    game_objects[n]->add_light(0.1, 0.8, 1.0, 1.0, 0.09, 0.032);
    game_objects[n]->name = "Object_" + std::to_string(n);

    num_lights++;
    renderer.setNumLights(num_lights);
}

void App::processMouseMovement(float mouse_xpos, float mouse_ypos) {
    // Check which regular GameObjects the mouse is hovering over
    mouseObjectsIntersect(mouse_xpos, mouse_ypos);

    // Check which gizmos the mouse is hovering over if an object is currently not
    // selected, no gizmo visible, don't bother with checks. Avoid checks if mouse
    // is being pressed
    if (selected_object && !mouse_pressed) {
        mouseGizmosIntersect(mouse_xpos, mouse_ypos);
    }

    // Mouse is over gizmo and mouse button is being pressed. Process gizmo function
    if (mouseover_gizmo && mouse_pressed) {
        glm::vec3 mouse_ray = mouseRaycast(mouse_xpos, mouse_ypos);

        std::string name = mouseover_gizmo->body->name;

        mouseover_gizmo->transformation_function(active_camera->pos, mouse_ray, selected_object,
                                                 previous_position, using_gizmo);
    }
}

void App::processScreenResize(float new_window_x, float new_window_y) {
    window_x = static_cast<int>(new_window_x);
    window_y = static_cast<int>(new_window_y);

    renderer.processScreenResize(window_x, window_y);
}

glm::vec3 App::mouseRaycast(float mouse_x, float mouse_y) {
    // Mouse position in x and y with it's z at the very far end of the
    // NDC space
    glm::vec3 mouse_ndc((2.0f * mouse_x - window_x) / window_x,
                        (window_y - 2.0f * mouse_y) / window_y, 1.0f);

    // Mouse in clip space with x and y, z is set at just one unit in front of the camera
    // w coordinate set to 1 to keep things simple
    glm::vec4 mouse_clip = glm::vec4(mouse_ndc.x, mouse_ndc.y, 1.0, 1.0);

    glm::mat4 projection = glm::perspective(
        glm::radians(active_camera->fov),
        (static_cast<float>(window_x) / static_cast<float>(window_y)), 0.1f, 100.f);

    glm::mat4 view = active_camera->lookAt();

    // Find position of the mouse in world space coordinates using the inverse
    // transformation of clip space -> world space
    glm::vec4 mouse_world = glm::inverse(projection * view) * mouse_clip;
    mouse_world /= mouse_world.w;

    // Find the direction of the vector pointing FROM the camera TO the mouse
    glm::vec3 mouse_ray_direction = glm::normalize(glm::vec3(mouse_world) - active_camera->pos);

    return mouse_ray_direction;
}

void App::mouseObjectsIntersect(float mouse_x, float mouse_y) {
    glm::vec3 mouse_direction = mouseRaycast(mouse_x, mouse_y);

    // Keep track of whether any object is under the mouse
    bool mouseover = false;

    // As this function essentially checks to see if an object should become
    // the mouseover object, will ignore completely if using a gizmo
    if (using_gizmo) {
        return;
    }

    for (unsigned int i = 0; i < game_objects.size(); ++i) {
        if (Math::rayBoundingBoxIntersection(active_camera->pos, mouse_direction,
                                             game_objects[i]->bbox)) {
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

void App::mouseGizmosIntersect(float mouse_x, float mouse_y) {
    // Check if the mouse is hovering over the positions arrows
    glm::vec3 mouse_direction = mouseRaycast(mouse_x, mouse_y);

    bool mouse_over_gizmo = false;

    for (auto& it : gizmos) {
        if (!it.second->getActivity()) {
            // If inactive, don't check
            continue;
        }
        if (Math::rayBoundingBoxIntersection(active_camera->pos, mouse_direction,
                                             it.second->body->bbox)) {
            mouseover_gizmo  = it.second;
            mouse_over_gizmo = true;
        }
    }

    if (!mouse_over_gizmo) {
        mouseover_gizmo = nullptr;
    }
}
