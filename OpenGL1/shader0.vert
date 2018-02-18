#version 330 core
layout (location = 0) in vec3 a_pos; // position var has attr position 0
layout (location = 1) in vec2 a_tex_coord; // texture coordinates in pos 1

out vec2 tex_coord; // Pipe texture coordinates to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    gl_Position = proj * view * model * vec4(a_pos, 1.0); // transform closest to vector is applied first
	tex_coord = a_tex_coord;
}