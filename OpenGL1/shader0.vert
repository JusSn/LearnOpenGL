#version 330 core
layout (location = 0) in vec3 a_pos; // position var has attr position 0
layout (location = 1) in vec3 a_color; // color is located in position 1
layout (location = 2) in vec2 a_tex_coord; // texture coordinates in pos 2

out vec3 vertex_color; // color output piped to fragment shader
out vec2 tex_coord;

void main() {
    gl_Position = vec4(a_pos, 1.0); // directly provide vec3 to vec4 constructor
    vertex_color = a_color;
	tex_coord = a_tex_coord;
}