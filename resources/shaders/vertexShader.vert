#version 450
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

layout(location = 0) out vec4 vertColor;

layout( push_constant ) uniform constants
{
    vec4 data;
    mat4 renderMatrix;
} PushConstants;
void main() {
    gl_Position = PushConstants.renderMatrix * vec4(position, 1);
    vertColor = color;
}