
#version 330

layout (location = 0) in vec3 a_vertex; 
layout (location = 2) in vec2 a_texcoord;

uniform mat4 projection;
uniform mat4 view;

out vec2 st;

void main(void) {
	vec4 vertex = vec4(a_vertex, 1.0);
	gl_Position = projection * view * vertex;

	st = a_texcoord;
	st.y = 1 - st.y;
	st.x = 1 - st.x;
}