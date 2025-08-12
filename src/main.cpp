#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <shader.hpp>
#include <camera.hpp>
#include <engine.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double mouse_xpos, double mouse_ypos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);

// -------------!!!--NOTE--!!!-------------
// The order of certain things such as defining the shape of our triangle or the
// shaders used may change depending on the program e.g. a game where appearances
// and positions change frequently.

// Window size
const float window_x = 1900.f;
const float window_y = 1080.f;

// Camera
Camera camera;

// Engine
Engine engine(window_x, window_y);

// Timings
float delta_time = 0.0f; // Time between current frame and last frame
float last_frame = 0.0f; // Time of last frame

// Mouse values
float last_mousex = window_x / 2.f; // Set to the middle since cursor wills start there
float last_mousey = window_y / 2.f;
bool first_mouse = true; // Keep track of whether mouse has been moved at all

double scroll_posy;
double last_scrolly;
bool first_scroll = true;

bool mouse_locked_invisible = true;
bool m_key_pressed = false;
bool mouse_button_pressed = false;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); Needed for Mac OS X

    // Create GLFW window object to old window data
    GLFWwindow* window = glfwCreateWindow(static_cast<int>(window_x), static_cast<int>(window_y), "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Lock the mouse in place into the middle of the window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Set a callback function for whenever the window senses the mouse has been moved
    // and or scrolled
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Initialialise GLAD in order to get access to OpenGL function pointers before
    //using OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Let OpenGL know the size of the viewport (can make it smaller if we want 
    // to display other things in the GLFW window
    glViewport(0, 0, static_cast<int>(window_x), static_cast<int>(window_y)); // x-pos of lower left corner, y-pos, width, height

    // Sets a "callback function" to be called whenever the GLFW framebuffer of the 
    // window specfied is resized. Called when window is first displayed.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // *----------------------------------------------------------------------------------*
    // | Depth Testing:  -OpenGL stores all DEPTH information within the DEPTH BUFFER     |
    // |   -Depth is stored for each fragment. When a fragment's colour is going to be    |
    // |   drawn, it's depth is compared to see if it is behind anything else.            |
    // |   -To make sure OpenGL is actually performing depth testing, nweed to enable it  |
    // |   using glEnable. This is used to enable many other switches other than depthtest|
    // |   -NOTE: Also need to refresh the depth buffer every frame to avoid previous a   |
    // |   previous frames buffer being used. Clear similar to colour buffer.             |
    // *----------------------------------------------------------------------------------*

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    // Imgui Code
    // ----------------------------------------------------------------------------------

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialise the engine
    engine.init();

    // Start the render loop which runs until GLWF window is closed
    // ----------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {

        // Keep track of frame time to ensure camera speeds are the same no matter
        // how many FPS your system is running at. I.e. each frame, multiply the velocity
        // of the camera by the time passed between frames to obtain the correct distance
        // travelled that frame
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        //Process inputs every frame
        processInput(window);

        // Set the colour that the buffer will be cleared with.
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // State setting function

        // Clear buffer with colour set
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // State using function

        // Imgui
        // ------------------------------------------------------------------------------
        // After clearing the OpenGL buffer, need to let ImGui know that we are now going
        // to work on the new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Engine rendering
        engine.render();

        // After drawing OpenGL objects, draw ImGUI
        engine.render_imgui();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the front and back buffer (look up Double Buffer)
        glfwSwapBuffers(window);

        // Checks if any events have been triggered e.g. keyboard input or mouse movement
        glfwPollEvents();

        // Update the engine
        engine.update();
    }

    glfwTerminate(); // Clean up all of GLFW resources

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Update the OpenGL viewport and engine whenever GLFW window is resized.
    engine.set_window_size(width, height);
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double mouse_xpos, double mouse_ypos) {
    // Function that is called whenever the mouse is moved

    float xpos = static_cast<float>(mouse_xpos);
    float ypos = static_cast<float>(mouse_ypos);

    // If mouse is freed
    if (!mouse_locked_invisible) {
        engine.mouseObjectsIntersect(xpos, ypos);
        return;
    }

    if (first_mouse) {
        // Cursor may sometimes start somewhere other than the middle before it is
        // "locked" into the centre. This may lead to flicks when the mouse is first
        // moved. This check prevents that.
        xpos = last_mousex;
        ypos = last_mousey;

        first_mouse = false;
    }

    float offset_x = xpos - last_mousex;
    float offset_y = ypos - last_mousey;
    last_mousex = mouse_xpos;
    last_mousey = mouse_ypos;

    engine.active_camera->processMouse(offset_x, offset_y);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    // Function that is called whenever scroll receives input

    engine.active_camera->processScroll(x_offset, y_offset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouse_button_pressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        if (mouse_button_pressed == true) {
            mouse_button_pressed = false;
            engine.processMouseClick();
        }
    }
}

void processInput(GLFWwindow* window) {
    // Function that will handle inputs e.g. keyboard key presses

    // glfwGetKey takes in current window and a certain key, if key being pressed,
    // GLFW_PRESS is returned, if not GLFW_RELEASE is returned
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        // Set Window to close
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        engine.active_camera->processKeyboard(FORWARD, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        engine.active_camera->processKeyboard(LEFT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        engine.active_camera->processKeyboard(BACKWARD, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        engine.active_camera->processKeyboard(RIGHT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        engine.active_camera->processKeyboard(UP, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        engine.active_camera->processKeyboard(DOWN, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        m_key_pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
        if (m_key_pressed) {
            if (mouse_locked_invisible) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                mouse_locked_invisible = false;
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                mouse_locked_invisible = true;
                // Reset mouse position to centre
                first_mouse = true;
            }
        }
        m_key_pressed = false;
    }
}
