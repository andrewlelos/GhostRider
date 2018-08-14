#version 330

in vec2 texCoords;
out vec4 fragColour;
uniform sampler2D guiTex;

void main(void){
    fragColour = texture(guiTex, texCoords);
}