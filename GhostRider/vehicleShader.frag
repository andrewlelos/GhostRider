
#version 330

in vec2 st;

out vec4 fragColour;

uniform sampler2D texMap;

void main(void) {
    fragColour = texture(texMap, st);
}