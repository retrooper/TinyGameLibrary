#version 450
//Output attributes
layout(location = 0) out vec4 outColor;
//Input attributes
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragViewVec;
layout(location = 3) in vec3 fragLightPos;
layout(location = 4) in vec3 fragWorldPos;

const float shininess = 16.0;
const float lightPower = 1;
const float screenGamma = 1;
const vec3 lightColor = vec3(1, 0.4, 0.1);
const vec3 specColor = vec3(1, 0.4, 0.1);
const int mode = 1; //BLIN-PHONG = 0, PHONG = 1
void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize((fragLightPos - fragWorldPos));
    vec3 viewDir = normalize(fragViewVec);
    float distance = length(lightDir);
    distance = distance * distance;
    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular = 0.0;
    float specAngle = 0.0;
    if (lambertian > 0.0) {
        // this is blinn phong
        if (mode == 0) {
            vec3 halfDir = normalize(lightDir + viewDir);
            specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, shininess);
        }
        // this is phong (for comparison)
        else if (mode == 1) {
            vec3 reflectDir = reflect(-lightDir, normal);
            specAngle = max(dot(reflectDir, viewDir), 0.0);
            // note that the exponent is different here
            specular = pow(specAngle, shininess/4.0);
        }
    }
    vec3 colorLinear = (fragColor).xyz * lambertian * lightColor * lightPower / distance + specColor * specular * lightColor * lightPower / distance;
    // apply gamma correction (assume ambientColor, diffuseColor and specColor
    // have been linearized, i.e. have no gamma correction in them)
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / screenGamma));
    // use the gamma corrected color in the fragment
    outColor = vec4(colorGammaCorrected, 1.0);
}