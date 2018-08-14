#version 330

in vec3 texCoords;
out vec4 fragColour;

uniform samplerCube cubeMap;

void main(void){
	fragColour = texture(cubeMap, texCoords);
}