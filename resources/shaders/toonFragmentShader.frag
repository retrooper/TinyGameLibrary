#version 450
//Output attributes
layout(location = 0) out vec4 outColor;
//Input attributes
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragViewVec;
layout(location = 3) in vec3 fragLightPos;
layout(location = 4) in vec3 fragWorldPos;
layout(location = 5) in float iTime;
layout(location = 6) in vec4 vertLoc;

const float highlightSize = 0.1;
const float shadowSize = 2;
const float outlineWidth = 0.1;
void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(fragLightPos - fragWorldPos);
    vec3 viewDir = normalize(fragViewVec);
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec4 color = fragColor;
    if (lambertian > 1.0-highlightSize) color = color * vec4(0.3, 0.3, 0.3, 2);
    if (lambertian > shadowSize) color = color * vec4(0, 0, 0, 0.9);
    if (dot(normal,viewDir) < outlineWidth) color = color * vec4(0.05,0.05,0.05,1);

    outColor = color;
}