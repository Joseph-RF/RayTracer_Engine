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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double mouse_xpos, double mouse_ypos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void processInput(GLFWwindow* window);

// -------------!!!--NOTE--!!!-------------
// The order of certain things such as defining the shape of our triangle or the
// shaders used may change depending on the program e.g. a game where appearances
// and positions change frequently.

// Window size
const float window_x = 800.f;
const float window_y = 600.f;

// Camera
Camera camera;

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

    // Build, compile and configure shader program
    // ----------------------------------------------------------------------------------
    Shader shader(RESOURCES_PATH "vshader.glsl", RESOURCES_PATH "fshader.glsl");


    // Set up vertex data, buffers and configure the vertex attributes
    // ----------------------------------------------------------------------------------

    // Define the vertices of the two triangles we want to draw to create rectangle
    float vertices[] = {
         0.0f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
    };

    glm::vec3 triangle_colour(1.0f, 0.0f, 0.0f); // Starts as red

    // Manage the memory where these vertices will go in the GPU using 
    // a vertex buffer obect (VBO). Able to store large batches of data on
    // the GPU where the GPU will have easy access
    unsigned int VBO; // ID of our VBO which we will use to access it
    unsigned int VAO; // Similar for VAO

    // Use Vertex Array Object (VAO) to keep the vertex attribute config saved in that VAO
    glGenVertexArrays(1, &VAO);
    // OpenGL function to generate buffer object. Sets the VBO equal to reference ID
    glGenBuffers(1, &VBO);

    // Bind the VAO first, then bind and set the vertex buffers. Finally configure vertex
    // attributes
    glBindVertexArray(VAO);

    // Binds buffer object to a specfic type, in this case, array buffer
    // Any buffer calls we make to GL_ARRAY_BUFFER will configure the bound object (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copies the user-defined data into the chose buffer.
    // 4th parameter defines how we want GPU to manage the data.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Need to tell OpenGL how to interpret the vertex data (configure vertex attribute)
    // Currently laid out like this:
    // [x1][y1][z1][x2][y2][z2][x3][y3][z3]
    // [ vertex_1 ][ vertex_2 ][ vertex_3 ]
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Argument 1: Specify which vertex attribute we want to configure. 
    // Recall location = 0 in the vertex shader source code
    // Argument 2: Size of vertex attribute (vec3 in the input)
    // Argument 3: Data type
    // Argument 5: Space between subsequent vertex attributes in bytes
    // Vertex attribute taken from the VBO currently bound to GL_ARRAY_BUFFER when 
    // function above is called
    glEnableVertexAttribArray(0); // Enables vertex attribute at specified location

    // Can now unbind VBO from GL_ARRAY_BUFFER as VBO was registered during
    // glVertexAttribPointer call. Not usually necessary. This is an example.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Can now unbind VAO to prevent future calls accidentally modifying this VAO
    // Not usually necessary. This is an example.
    glBindVertexArray(0);


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

    // Imgui Code
    // ----------------------------------------------------------------------------------

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


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

        // Sets given shader program as active shader program to use for drawing commands
        shader.use();


        // Define the transformations
        // ------------------------------------------------------------------------------

        // Model transformation taking objects from their local coordinates to global
        // coordinates. Objects imported from blender usually have 0,0,0 in their centre
        // we may want to place an object somewhere other than the centre of the scene.
        glm::mat4 model(1.0f);

        // View transformation: Changes coordinates such that everything is in front of
        // of the camera / user
        // For now will just take a step back, i.e. move the entire scene forwards.
        // OpenGL is righthanded. I.e. right is +x, up is +y, forward is -z
        // *----------------------------------------------------------------------------------*
        // | Camera:  -Will simulate the effect of a camera in OpenGL using view transform    |
        // |   -To simplify the process will use GLM's lookat function which creates a lookat |
        // |   matrix that defines the camera's aim and POSITION in space.                    |
        // |   -Mathematically, lookat matrix created using two matrices, one for aim the     |
        // |   other for position. Aim matrix is transposed and position is negated since     |
        // |   moving the camera is equivalent to moving the entire scene which is what we    |
        // |   are actually going to do.                                                      |
        // *----------------------------------------------------------------------------------*

        glm::mat4 view = camera.lookAt();

        // Projection transformation: Move everything from view space to clip space.
        // I.e. need to move things to the standard coordinates of -1.0 to 1.0.
        // Defined by a viewing box called frustum where only vertices inside end up
        // beind rendered. Can do this using ORTHOGRAPHIC or PERSPECTIVE projection.
        // Perspective is the realistic one so will use that
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.fov), (window_x / window_y), 0.1f, 100.f);
        // Argument 1: FoV
        // Argument 2: Aspect ratio of window
        // Argument 3: Near plane of frustum
        // Argument 4: Far plane of frustum

        // Apply the transformations
        // ------------------------------------------------------------------------------
        shader.setMat("model", model);

        shader.setMat("view", view);

        shader.setMat("projection", projection);


        // Set the colour that the buffer will be cleared with.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // State setting function

        // Clear buffer with colour set
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // State using function

        // Imgui
        // ------------------------------------------------------------------------------
        // After clearing the OpenGL buffer, need to let ImGui know that we are now going
        // to work on the new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        shader.setVec3("colour", triangle_colour);

        // Want to draw the desired triangle now, bind VAO defined earlier
        glBindVertexArray(VAO);
        // Draws primitives using currently active shader, vertex config from current 
        // Vertex Array Object and VBO indirectly bound by VAO
        // Argument 1: Type of primitive we want to draw
        // Argument 2: Starting point of data
        // Argument 3: Number of vertices
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // After drawing OpenGL objects, draw ImGUI
        ImGui::Begin("ImGUI Window");
        ImGui::Text("This is an ImGUI window!");
        ImGui::ColorEdit3("Triangle Colour", glm::value_ptr(triangle_colour));
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the front and back buffer (look up Double Buffer)
        glfwSwapBuffers(window);

        // Checks if any events have been triggered e.g. keyboard input or mouse movement
        glfwPollEvents();
    }

    glfwTerminate(); // Clean up all of GLFW resources

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Update the OpenGL viewport whenever GLFW window is resized.
    // Can do anything in this function (if we want to)
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double mouse_xpos, double mouse_ypos) {
    // Function that is called whenever the mouse is moved

    float xpos = static_cast<float>(mouse_xpos);
    float ypos = static_cast<float>(mouse_ypos);

    // If mouse is freed, ignore this function
    if (!mouse_locked_invisible) {
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

    camera.processMouse(offset_x, offset_y);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    // Function that is called whenever scroll receives input

    camera.processScroll(x_offset, y_offset);
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
        camera.processKeyboard(FORWARD, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(LEFT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(BACKWARD, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(RIGHT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.processKeyboard(UP, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.processKeyboard(DOWN, delta_time);
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
