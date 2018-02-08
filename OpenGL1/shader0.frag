#version 330 core
out vec4 frag_color;

in vec3 vertex_color; // input from vx shader
in vec2 tex_coord; // texture coordinates

//fragment shader is the one that actually sets pixels to corresponding color from texture
uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    frag_color = mix(texture(texture1, tex_coord), texture(texture2, tex_coord), 0.2);
}