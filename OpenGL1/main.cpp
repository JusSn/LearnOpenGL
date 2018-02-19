/* Code based off tutorials from https://learnopengl.com */

#include "Shader.h" // Custom shader class to quickly compile and link vertex + fragment shader
#include "Camera.h"
#include "stb_image_.h" // Sean Barret's image loader lib

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// opengl math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

// Viewport dimensions
const int kWidth{ 800 };
const int kHeight{ 600 };

// Register callback on window that gets called every time window is resized
static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
// Register mouse position callback
static void MouseCallback(GLFWwindow* window, double x_pos, double y_pos);
// Register mouse scroll callback
static void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset);

// Process input during render loop
static void ProcessInput(GLFWwindow* window, double delta_time);

// load an image into currently bound texture
static unsigned int CreateTexture2D(GLenum tex_unit, char* filename);

int main() {
    glfwInit();

    // Configure GLFW
    // Use opengl 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // use opengl core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window obj
    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Tell GLFW to make the context of our window the main context on UI thread
    glfwMakeContextCurrent(window);

    // Must initialize GLAD before calling OpenGL funcs, since GLAD manages func ptrs
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to init GLAD!" << std::endl;
        return -1;
    }

    // Capture cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Register callback functions
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    
    /* Textures */

    // Generate ogl texture object
    unsigned int container_tex{ CreateTexture2D(GL_TEXTURE0, "container.jpg") };

    // load second image
    unsigned int face_tex{ CreateTexture2D(GL_TEXTURE1, "awesomeface.png") };

    /* GPU Pipeline begins? */

    // Create vertex and fragment shaders from file; compile and link into shader program
    Shader shader_program{ "shader0.vert", "shader0.frag" };

    /* 1) Copy array into buffer for OpenGL */

    // Cube for texture tutorial. Stores local space vertices and texture coords
    float vertices[] {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    // Supply 10 locations for 10 cubes
    glm::vec3 cubePositions[] {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // OpenGL core requires vertex array object as well
    // VAO stores the vertex attr config and which VBO to use
    unsigned int vx_buf_obj, vx_array_obj;
    glGenVertexArrays(1, &vx_array_obj);
    glGenBuffers(1, &vx_buf_obj);

    /* 1. bind VAO; only changes if object changes */
    glBindVertexArray(vx_array_obj);
    // Now any calls we make on GL_ARRAY_BUFFER will target vxBufObj
    glBindBuffer(GL_ARRAY_BUFFER, vx_buf_obj);

    // Copies vertex data for triangle 1 into the bound VBO
    // _DNYAMIC_DRAW will change a lot, _STREAM_DRAW changes every time its drawn
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

    /* 2) Set vertex attributes pointers */
    // tell opengl how to interpret our vertex data (how it's packed in the VBO)
    // Note stride can be left as zero if data is tightly packed
    // Note this is called when vx_buf_obj is bound to GL_ARRAY_BUFFER

    // Set position attribute from idx 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    // Set texture attribute from idx 1; specify the attribute offset
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Note at this point, array buffer can be unbound. 
    // Remember, DO NOT unbind the EBO while a VAO is active; bound EBO is stored within a VBO.
    // VAO can be unbound afterwards so that later VAO calls won't modify the wrong one

    // 3) Now set to draw the object

    // Set texture unit sampler uniforms
    // Must activate the shader program before setting uniforms!
    shader_program.Use();
    shader_program.SetInt("texture1", 0); // Active texture 0
    shader_program.SetInt("texture2", 1);

    // Tell opengl not to draw obscured vertices
    glEnable(GL_DEPTH_TEST);

    /* RENDER LOOP */

    // track deltaTime
    double delta_time{};
    double last_frame_time{}; 

    while (!glfwWindowShouldClose(window)) { // returns true when window is closed by user
        // update deltaTime
        double current_frame_time{ glfwGetTime() };
        delta_time = current_frame_time - last_frame_time;
        last_frame_time = current_frame_time;

        // check for user input
        ProcessInput(window, delta_time);

        /* RENDERING COMMANDS GO HERE */

        // Let's clear the screen with a greenish-blue; set clear color
        glClearColor(0.2f, 0.3f, 0.3f, 1.f);
        // fills colorbuffer with the color configured by glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up transformation matrix 
        // remember that transformations are applied in reverse order from code
        // b) View Transform
        auto view = Camera::GetInstance().GetViewTransform();
        // c) Projection Transform: FOV, aspect ratio, near clipping plane, far clipping plane
        auto proj = glm::perspective(glm::radians(Camera::GetInstance().GetZoom()), double{ kWidth } / kHeight, 0.1, 100.);

        glBindVertexArray(vx_array_obj);

        // 4) activate program obj; update transform matrices in vertex shader
        shader_program.Use();
        shader_program.SetMatrix4("view", view);
        shader_program.SetMatrix4("proj", proj);
        
        // 5) Draw each cube each with a different rotation
        for (int i{}; i < 10; ++i) {
            // a) Model Transform
            auto model = glm::translate(glm::mat4{ 1.f }, cubePositions[i]);
            // angle of rotation, and arbitrary axis. Rotate over time
            float angle{ 20.f * (i + 1) };
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3{ 1.f, 0.3f, 0.5f });
            shader_program.SetMatrix4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        /* glfw: swap buffers and poll I/O */
        // swap front and back buffer
        glfwSwapBuffers(window);
        // Checks if events are triggered, calls corresponding functions set by callbacks
        glfwPollEvents();
    }

    // cleans/deletes all allocated resources
    glfwTerminate();
    return 0;
}

/* HELPER FUNCTIONS */

// Register callback on window that gets called every time window is resized
void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // Tell OpenGL size of rendering viewport
    glViewport(0, 0, width, height);
}

// Register mouse position callback
static void MouseCallback(GLFWwindow* window, double x_pos, double y_pos) {
    Camera::GetInstance().Look(x_pos, y_pos);
}
// Register mouse scroll callback
static void ScrollCallback(GLFWwindow* window, double x_offset, double y_offset) {
    Camera::GetInstance().Zoom(y_offset);
}

// Process input during render loop
void ProcessInput(GLFWwindow* window, double delta_time) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Camera manipulation
    Camera& camera{ Camera::GetInstance() };
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.Move(CameraMove::kForward, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.Move(CameraMove::kLeft, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.Move(CameraMove::kBackward, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.Move(CameraMove::kRight, delta_time);
    }
}

// load an image into currently bound texture
unsigned int CreateTexture2D(GLenum tex_unit, char* filename) {
    unsigned int tex;
    glGenTextures(1, &tex);
    // Specify texture unit this image will occupy
    glActiveTexture(tex_unit);
    glBindTexture(GL_TEXTURE_2D, tex);
    // Set wrap/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S and T are texture dimension vars
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image
    int width, height, num_channels;
    unsigned char* img_data{ stbi_load(filename, &width, &height, &num_channels, 0) };

    // Check for success
    if (img_data) {
        // If png, specify transparency format
        size_t len{ strlen(filename) };
        filename += (len - 3); // For file extensions differen in last 3 chars
        int format{ (strcmp("jpg", filename) == 0) ? GL_RGB : GL_RGBA };
        // apply image to 2D texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, img_data);
        // Mipmap for bound texture
        glGenerateMipmap(GL_TEXTURE_2D);

        // Good idea to free the image memory after generating mipmaps
        stbi_image_free(img_data);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    return tex;
}
