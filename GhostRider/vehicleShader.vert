
#version 330

layout (location = 0) in vec3 a_vertex; 
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normal_matrix;

vec4 vertex;
out vec2 st;

void main(void) {
	vertex = vec4(a_vertex, 1.0);
	gl_Position = projection * view * model * vertex;
	st = a_texcoord;
	// Invert the Y value of the texture coords.
	st.y = 1.0 - st.y;
}