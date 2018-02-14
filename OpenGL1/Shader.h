/* 
Shader class to read GLSL from file and build shader programs
Based off example from https://learnopengl.com/Getting-started/Shaders
*/

#ifndef SHADER_H
#define SHADER_H

#include <glm/gtc/type_ptr.hpp> // for glm::mat4

class Shader {
    // Program id
    unsigned int id;
public:
    // Prevent any funny business with uninitialized shaders
    Shader() = delete;
    // Builds shader program using specified vertex and fragment shaders from files
    // Throws exception if file cannot be read
    Shader(const char* vertex_path, const char* frag_path);
    // Use the program maintained by this Shader instance
    void use() const;
    
    // Functions to modify uniforms
    void setBool(const char* name, bool val) const;
    void setInt(const char* name, int val) const;
    void setFloat(const char* name, float val) const;
    void setMatrix4(const char* name, glm::mat4 trans);
};

#endif // !SHADER_H

