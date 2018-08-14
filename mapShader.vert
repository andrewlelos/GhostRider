
#version 330

layout (location = 0) in vec3 a_vertex; 
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normal_matrix;

out float visibility;

out vec4 vertex;
out vec3 normal;
out vec2 st;

const float density = 0.05;
const float gradient = 1.9;

void main(void) {
	vertex = vec4(a_vertex, 1.0);
	normal = normalize(normal_matrix * a_normal);
	gl_Position = projection * view * model * vertex;
	st = a_texcoord;

	// Invert the Y value of the texture coords.
	st.y = 1.0 - st.y;

	vec4 positionRelToCam = view * vertex;

	float distance = length(positionRelToCam.xyz);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}