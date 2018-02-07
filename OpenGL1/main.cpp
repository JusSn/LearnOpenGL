#include "Shader.h" // Custom shader class to quickly compile and link vertex + fragment shader

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

    // Create vertex and fragment shaders from file; compile and link into shader program
    Shader shader_program{ "shader0.vert", "shader0.frag" };

    /* 1) Copy array into buffer for OpenGL */
    float vertices[] = {
        // positions         // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };
    //unsigned int indices[] {
    //    0, 1, 3,   // first triangle
    //    1, 2, 3    // second triangle
    //};

    // OpenGL core requires vertex array object as well
    // VAO stores the vertex attr config and which VBO to use
    unsigned int vx_buf_obj, vx_array_obj;// elem_buffer_obj;
    glGenVertexArrays(1, &vx_array_obj);
    glGenBuffers(1, &vx_buf_obj);
    //glGenBuffers(1, &elem_buffer_obj);

    /* 1. bind VAO; only changes if object changes */
    glBindVertexArray(vx_array_obj);
    // Now any calls we make on GL_ARRAY_BUFFER will target vxBufObj
    glBindBuffer(GL_ARRAY_BUFFER, vx_buf_obj);

    // Copies vertex data for triangle 1 into the bound VBO
    // _DNYAMIC_DRAW will change a lot, _STREAM_DRAW changes every time its drawn
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

    // Now set attributes for element buffer object
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buffer_obj);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /* 2) Set vertex attributes pointers */
    // tell opengl how to interpret our vertex data (how it's packed in the VBO)
    // Note stride can be left as zero if data is tightly packed
    // Note this is called when vx_buf_obj is bound to GL_ARRAY_BUFFER

    // Set position attribute from idx 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set color attribute from idx 1; specify the attribute offset
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Note at this point, array buffer can be unbound. 
    // Remember, DO NOT unbind the EBO while a VAO is active; bound EBO is stored within a VBO.
    // VAO can be unbound afterwards so that later VAO calls won't modify the wrong one

    // 3) Now set to draw the object

    /* RENDER LOOP */
    while (!glfwWindowShouldClose(window)) { // returns true when window is closed by user
        // check for user input
        processInput(window);

        /* RENDERING COMMANDS GO HERE */

        // Let's clear the screen with a greenish-blue; set clear color
        glClearColor(0.2f, 0.3f, 0.3f, 1.f);
        // fills colorbuffer with the color configured by glClearColor
        glClear(GL_COLOR_BUFFER_BIT);

        // 4) activate program obj; draw the triangle
        shader_program.use();
        glBindVertexArray(vx_array_obj);
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
