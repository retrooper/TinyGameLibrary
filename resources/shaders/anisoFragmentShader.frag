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

const vec3 SpecDirection = vec3(0, 1, 0);
const float Roughness = 10;
const float AnisoRoughness = 20;
const float shininess = 8.0;
const float lightPower = 1.5;
const float screenGamma = 1;
const vec3 lightColor = vec3(1, 0.4, 0.1);
const vec3 specColor = vec3(1, 0.4, 0.1);
void main() {
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(fragViewVec);
    vec3 lightDir = normalize(fragLightPos - fragWorldPos);
    float distance = length(lightDir);
    distance = distance * distance;
    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular = 0.0;
    float specAngle = 0.0;
    if (lambertian > 0.0) {
        vec3 reflectDir = reflect(-lightDir, normal);
        specAngle = max(dot(reflectDir, viewDir), 0.0);
        // note that the exponent is different here
        specular = pow(specAngle, shininess/4.0);
    }
    vec3 colorLinear = (fragColor).xyz * lambertian * lightColor * lightPower / distance + specColor * specular * lightColor * lightPower / distance;
    // apply gamma correction (assume ambientColor, diffuseColor and specColor
    // have been linearized, i.e. have no gamma correction in them)
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / screenGamma));
    vec3 v = normalize(fragWorldPos);
    vec3 t = cross(normal,normalize(SpecDirection));
    vec3 h = normalize(lightDir+fragWorldPos);

    // Heidrich-Seidel anisotropic distribution
    float ldott = dot(lightDir,t);
    float vdott = dot(viewDir,t);

    float aniso = pow(sin(ldott)*sin(vdott) +
    cos(ldott)*cos(vdott),1.0/AnisoRoughness);

    outColor = vec4(colorGammaCorrected, 1) * aniso;
}