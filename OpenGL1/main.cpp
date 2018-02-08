/* Code based off tutorials from https://learnopengl.com */

#include "Shader.h" // Custom shader class to quickly compile and link vertex + fragment shader
#include "stb_image_.h" // Sean Barret's image loader lib

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Register callback on window that gets called every time window is resized
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Process input during render loop
static void processInput(GLFWwindow* window);

// load an image into currently bound texture
static unsigned int createTexture(GLenum tex_unit, char* filename);

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
    
    /* Textures */

    // Generate ogl texture object
    unsigned int container_tex{ createTexture(GL_TEXTURE0, "container.jpg") };

    // load second image
    unsigned int face_tex{ createTexture(GL_TEXTURE1, "awesomeface.png") };

    /* GPU Pipeline begins? */

    // Create vertex and fragment shaders from file; compile and link into shader program
    Shader shader_program{ "shader0.vert", "shader0.frag" };

    /* 1) Copy array into buffer for OpenGL */

    // Rectangle for texture tutorial
    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // OpenGL core requires vertex array object as well
    // VAO stores the vertex attr config and which VBO to use
    unsigned int vx_buf_obj, vx_array_obj, elem_buffer_obj;
    glGenVertexArrays(1, &vx_array_obj);
    glGenBuffers(1, &vx_buf_obj);
    glGenBuffers(1, &elem_buffer_obj);

    /* 1. bind VAO; only changes if object changes */
    glBindVertexArray(vx_array_obj);
    // Now any calls we make on GL_ARRAY_BUFFER will target vxBufObj
    glBindBuffer(GL_ARRAY_BUFFER, vx_buf_obj);

    // Copies vertex data for triangle 1 into the bound VBO
    // _DNYAMIC_DRAW will change a lot, _STREAM_DRAW changes every time its drawn
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

    // Now set attributes for element buffer object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem_buffer_obj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /* 2) Set vertex attributes pointers */
    // tell opengl how to interpret our vertex data (how it's packed in the VBO)
    // Note stride can be left as zero if data is tightly packed
    // Note this is called when vx_buf_obj is bound to GL_ARRAY_BUFFER

    // Set position attribute from idx 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Set color attribute from idx 1; specify the attribute offset
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Notify OGL of texture attribute from idx 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Note at this point, array buffer can be unbound. 
    // Remember, DO NOT unbind the EBO while a VAO is active; bound EBO is stored within a VBO.
    // VAO can be unbound afterwards so that later VAO calls won't modify the wrong one

    // 3) Now set to draw the object

    // Set texture unit sampler uniforms
    // Must activate the shader program before setting uniforms!
    shader_program.use();
    shader_program.setInt("texture1", 0); // Active texture 0
    shader_program.setInt("texture2", 1);

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
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

// load an image into currently bound texture
unsigned int createTexture(GLenum tex_unit, char* filename) {
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
