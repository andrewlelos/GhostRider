#version 330

in vec2 position; 
out vec2 texCoords;

uniform mat4 model;

void main(void){
    gl_Position = model * vec4(position, 0.0, 1.0);
    texCoords = vec2((position.x + 1.0)/2.0, 1 - (position.y + 1.0)/2.0);
}