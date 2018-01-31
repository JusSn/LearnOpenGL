#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Register callback on window that gets called every time window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// Process input during render loop
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

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

    // Tell OpenGL size of rendering viewport
    // llcorner.x, llcorner.y, width, height
    glViewport(0, 0, 800, 600);

    // Register window resizing callback function
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Create render loop
    while (!glfwWindowShouldClose(window)) { // returns true when window is closed by user
        // check for user input
        processInput(window);

        /* RENDERING COMMANDS GO HERE */
        // Let's clear the screen with a greenish-blue; set clear color
        glClearColor(0.2f, 0.3f, 0.3f, 1.f);
        // fills colorbuffer with the color configured by glClearColor
        glClear(GL_COLOR_BUFFER_BIT);

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
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, 800, 600);
}