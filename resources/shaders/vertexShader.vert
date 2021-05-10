#version 450
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
//Output attributes
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragViewVec;
layout(location = 3) out vec3 fragLightPos;
layout(location = 4) out vec3 fragWorldPos;
layout(location = 5) out float iTime;
layout( push_constant ) uniform constants
{
   // mat4 view;
    mat4 projection;
} CameraData;


layout(binding = 0) uniform modeldata
{
    mat4 model;
    vec3 lightPos;
    float time;
} ModelData;
void main() {
    vec4 worldPos = ModelData.model * vec4(position, 1);
    //gl_Position = CameraData.projection * CameraData.view * worldPos;
    gl_Position = CameraData.projection * worldPos;
    fragColor = color;
    //fragUVCoord = uvCoord;
    fragNormal = mat3(ModelData.model) * normal;
    //fragViewVec = (CameraData.view * worldPos).xyz;
    fragViewVec = (worldPos).xyz;
    fragLightPos = ModelData.lightPos;
    fragWorldPos = (worldPos).xyz;
    iTime = ModelData.time;
}