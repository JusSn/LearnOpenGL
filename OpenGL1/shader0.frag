#version 330 core
out vec4 frag_color;

in vec3 vertex_color; // input from vx shader

void main() {
    frag_color = vec4(vertex_color, 1.0f);
}