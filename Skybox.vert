#version 330

layout (location = 0) in vec3 position;
out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main(void){
	gl_Position = projection * view * vec4(position, 1.0);
	texCoords = position;
}