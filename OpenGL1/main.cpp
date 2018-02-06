#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// vertex shader raw text
const char* vx_shader_src{ R"(
#version 330 core
layout (location = 0) in vec3 a_pos; // position var has attr position 0
layout (location = 1) in vec3 a_color; // color is located in position 1

out vec3 vertex_color; // color output piped to fragment shader

void main() {
    gl_Position = vec4(a_pos, 1.0); // directly provide vec3 to vec4 constructor
    vertex_color = a_color;
}
)"};

// fragment shader, rainbow
const char* frag_shader_src{ R"(
#version 330 core
out vec4 frag_color;

in vec3 vertex_color; // input from vx shader

void main() {
    frag_color = vec4(vertex_color, 1.0f);
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

    // For exercise: create second fragment shader of different color
    //unsigned int frag_shader_2{ glCreateShader(GL_FRAGMENT_SHADER) };
    //glShaderSource(frag_shader_2, 1, &uniform_shader_src, nullptr);
    //glCompileShader(frag_shader_2);

    // Link second shader prg
    //unsigned int shader_2{ glCreateProgram() };
    //glAttachShader(shader_2, vx_shader);
    //glAttachShader(shader_2, frag_shader_2);
    //glLinkProgram(shader_2);

    // check for linking errors
    //glGetProgramiv(shader_2, GL_LINK_STATUS, &success);
    //if (!success) {
    //    glGetProgramInfoLog(shader_2, 512, nullptr, info_log);
    //    std::cout << "ERROR [SHADER LINKING FAILED]\n" << info_log << std::endl;
    //}

    // shader objects are no longer needed in memory once linked
    glDeleteShader(vx_shader);
    glDeleteShader(frag_shader);
    //glDeleteShader(frag_shader_2);

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

    //float vert_2[]{
    //    -0.5f,  0.6f, 0.0f,   // top left 
    //    -0.5f,  -0.4f, 0.0f,  // bottom left
    //     0.5f,  0.6f, 0.0f    // top right
    //};
    
    // OpenGL core requires vertex array object as well
    // VAO stores the vertex attr config and which VBO to use
    unsigned int vx_buf_obj, vx_array_obj;// elem_buffer_obj;
    glGenVertexArrays(1, &vx_array_obj);
    glGenBuffers(1, &vx_buf_obj);
    //glGenBuffers(1, &elem_buffer_obj);

    // For exercise: second vbo/vao for second triangle
    //unsigned int vx_buf_2, vx_array_2;
    //glGenVertexArrays(1, &vx_array_2);
    //glGenBuffers(1, &vx_buf_2);

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

    // For exercise: bind second triangle
    //glBindVertexArray(vx_array_2);
    //glBindBuffer(GL_ARRAY_BUFFER, vx_buf_2); // drawing this array now draws this buffer
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vert_2), vert_2, GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

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
        glUseProgram(shader_program);
        glBindVertexArray(vx_array_obj);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // For exercise: draw second triangle
        // Change uniform color variable
        //float time_val = glfwGetTime(); // running time in seconds
        //float green_val = (sin(time_val) / 2.f) + 0.5f;
        // find location in memory for updating
        //int vx_color_loc = glGetUniformLocation(shader_2, "our_color");
        // updating a uniform sets the instance on the currently active shader program
        //glUseProgram(shader_2);
        //glUniform4f(vx_color_loc, 0.f, green_val, 0.f, 1.f);

        //glBindVertexArray(vx_array_2);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        // Instead of drawing from array, specify we are drawing by indices
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 

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
