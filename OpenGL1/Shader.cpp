/*
Shader class to read GLSL from file and build shader programs
Based off example from https://learnopengl.com/Getting-started/Shaders
*/

#include "Shader.h"

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

using std::string;
using std::ifstream;

// Helper function to process files into c-strings
// throws exception if file cannot be read
static const string readFileToString(const char* filepath);

/* Shader class implementation */

// Builds shader object using specified vertex and fragment shaders from files
Shader::Shader(const char* vertex_path, const char* fragment_path) {
    // Get vertex/fragment source code from their filepaths
    // Create strings on stack for char pointers
    string vx_string{ readFileToString(vertex_path) };
    string frag_string{ readFileToString(fragment_path) };

    const char* vx_shader_code{ vx_string.c_str() };
    const char* frag_shader_code{ frag_string.c_str() };

    // compile shaders

    // vertex shader 
    unsigned int vertex{ glCreateShader(GL_VERTEX_SHADER) };
    glShaderSource(vertex, 1, &vx_shader_code, nullptr);
    glCompileShader(vertex);

    // check for compile errors
    int success;
    char info_log[512];
    
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, info_log);
        std::cout << "ERROR [VERTEX SHADER COMPILATION FAILED]\n" << info_log << std::endl;
    }

    // fragment shader
    unsigned int fragment{ glCreateShader(GL_FRAGMENT_SHADER) };
    glShaderSource(fragment, 1, &frag_shader_code, nullptr);
    glCompileShader(fragment);

    // check for compile errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, info_log);
        std::cout << "ERROR [FRAGMENT SHADER COMPILATION FAILED]\n" << info_log << std::endl;
    }

    // Finally, link shader program
    id_ = glCreateProgram();
    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);
    glLinkProgram(id_);

    // Check for linkage errors
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id_, 512, nullptr, info_log);
        std::cout << "ERROR [SHADER PROGRAM LINKING FAILED]\n" << info_log << std::endl;
    }

    // Delete shaders as they are now linked
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// Use the program maintained by this Shader instance
void Shader::Use() const { glUseProgram(id_); }

// Uniform modifiers
void Shader::SetBool(const char* name, bool val) const {
    SetInt(name, val);
}
void Shader::SetInt(const char* name, int val) const {
    glUniform1i(glGetUniformLocation(id_, name), val);
}
void Shader::SetFloat(const char* name, float val) const {
    glUniform1f(glGetUniformLocation(id_, name), val);
}
void Shader::SetMatrix4(const char* name, glm::mat4 trans) {
    glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, GL_FALSE, &trans[0][0]);
}

/* Non-memeber helper implementation */

// Helper function to process files into c-strings
// throws exception if file cannot be read
const string readFileToString(const char* filepath) {
    ifstream shader_input;

    // ensure ifstreams can throw exceptions
    shader_input.exceptions(ifstream::failbit | ifstream::badbit);

    try {
        shader_input.open(filepath);
        std::stringstream shader_stream;

        // Convert ifstream -> stringstream -> string -> cstring
        shader_stream << shader_input.rdbuf();
        shader_input.close();

        return shader_stream.str();
    }
    catch (ifstream::failure& e) {
        std::cout << "ERROR [ " << e.what() << " ]\n" << filepath << std::endl;
    }
}