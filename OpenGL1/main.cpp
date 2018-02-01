#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// vertex shader raw text
const char* vx_shader_src{ R"(
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)"};

// fragment shader
const char* frag_shader_src{ R"(
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.f, 0.5f, 0.2f, 1.f);
}
)" };

// Register callback on window that gets called every time window is resized
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Process input during render loop
void processInput(GLFWwindow* window);

int main() {
    glfwInit();

    // Configure GLFW
    // Use opengl 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // use opengl core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window obj
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Tell GLFW to make the context of our window the main context on UI thread
    glfwMakeContextCurrent(window);

    // Must initialize GLAD before calling OpenGL funcs, since GLAD manages func ptrs
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD!" << std::endl;
        return -1;
    }

    // Register window resizing callback function
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    /* GPU Pipeline begins? */

    /* create and compile vertex shader, used to transform coords to NDC */
    unsigned int vx_shader{ glCreateShader(GL_VERTEX_SHADER) };

    glShaderSource(vx_shader, 1, &vx_shader_src, nullptr);
    glCompileShader(vx_shader);

    // Check for vx shader compile errors
    int success;
    char info_log[512];
    glGetShaderiv(vx_shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vx_shader, 512, nullptr, info_log);
        std::cout << "ERROR [VERTEX SHADER COMPILATION FAILED]\n" << info_log << std::endl;
    }

    /* create and compile fragment shader, used to decide pixel color */
    unsigned int frag_shader{ glCreateShader(GL_FRAGMENT_SHADER) };
    glShaderSource(frag_shader, 1, &frag_shader_src, nullptr);
    glCompileShader(frag_shader);

    // Link both compiled shaders to shader program obj
    // linking pipes output of one shader to input of the next
    unsigned int shader_program{ glCreateProgram() };
    glAttachShader(shader_program, vx_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);

    // check for linking errors
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
        std::cout << "ERROR [SHADER LINKING FAILED]\n" << info_log << std::endl;
    }

    // shader objects are no longer needed in memory once linked
    glDeleteShader(vx_shader);
    glDeleteShader(frag_shader);

    /* 1) Copy array into buffer for OpenGL */
    float vertices[]{
        -0.5f, -0.5f, 0.f,
        0.5f, -0.5f, 0.f,
        0.f, 0.5f, 0.f
    };
    
    // OpenGL core requires vertex array object as well
    // VAO stores the vertex attr config and which vbo to use
    unsigned int vx_buf_obj, vx_array_obj;
    glGenVertexArrays(1, &vx_array_obj);
    glGenBuffers(1, &vx_buf_obj);

    /* 1. bind VAO; only changes if object changes */
    glBindVertexArray(vx_array_obj);
    // Now any calls we make on GL_ARRAY_BUFFER will target vxBufObj
    glBindBuffer(GL_ARRAY_BUFFER, vx_buf_obj);

    // Copies data into the bound buffer
    // _DNYAMIC_DRAW will change a lot, _STREAM_DRAW changes every time its drawn
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

    /* 2) Set vertex attributes pointers */
    // tell opengl how to interpret our vertex data (how it's packed in mem)
    // Note stride can be left as zero if data is tightly packed
    // Note this is called when vx_buf_obj is bound to GL_ARRAY_BUFFER
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 3) activate program obj
    glUseProgram(shader_program);

    // 4) Now set to draw the object

    /* RENDER LOOP */
    while (!glfwWindowShouldClose(window)) { // returns true when window is closed by user
        // check for user input
        processInput(window);

        /* RENDERING COMMANDS GO HERE */

        // Let's clear the screen with a greenish-blue; set clear color
        glClearColor(0.2f, 0.3f, 0.3f, 1.f);
        // fills colorbuffer with the color configured by glClearColor
        glClear(GL_COLOR_BUFFER_BIT);

        // 3) activate program obj; draw the triangle
        glUseProgram(shader_program);
        glBindVertexArray(vx_array_obj); // technically unneeded since there's only one VAO
        glDrawArrays(GL_TRIANGLES, 0, 3);

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

// Register callback on window that gets called every time window is resized
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // Tell OpenGL size of rendering viewport
    glViewport(0, 0, width, height);
}

// Process input during render loop
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
