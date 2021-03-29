#version 450
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

layout(location = 0) out vec4 vertColor;
void main() {
    gl_Position = vec4(position, 1);
    vertColor = color;
}