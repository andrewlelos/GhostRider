
#version 330

layout (location = 0) in vec3 a_vertex; 
layout (location = 1) in vec3 a_normal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normal_matrix;

vec4 vertex;
out vec3 normal;

void main(void) {
	vertex = vec4(a_vertex, 1.0);
	normal = normalize(normal_matrix * a_normal);
	gl_Position = projection * view * model * vertex;
}