
#version 330

in vec3 normal;

out vec4 fragColour;

void main(void) {
    fragColour = (0.6 + 0.5 * vec4(normal, 1.0));
}