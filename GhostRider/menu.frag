
#version 330

in vec2 st;

uniform sampler2D texMap;

out vec4 fragColour;

void main(void) {
	vec4 textureColour = texture(texMap, st);
	vec4 colour = vec4(1.0, 1.0, 0.0, 1.0);
	fragColour = textureColour;
}