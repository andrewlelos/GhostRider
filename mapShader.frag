
#version 330

in vec4 vertex;
in vec3 normal;
in vec2 st;
in float visibility;

uniform vec3 lampPos;

uniform vec3 street_light_ambient;
uniform vec3 street_light_diffuse;
uniform vec3 street_light_specular;

uniform sampler2D texMap;

const float constantAttn = 0.005;
const float linearAttn = 0.0025;
const float quadraticAttn = 0.00125;

const float cutoff = 0.70710678118;

uniform int tetra;

out vec4 fragColour;

vec3 lampLight(in vec4 position, in vec3 norm) {
    vec3 light_pos = lampPos;
    vec3 lightDir = normalize(light_pos - vec3(position));
    vec3 normal = normalize(norm);

    float lambertian = max(dot(lightDir, normal), 0.0);
    float spec = 0.0;

    if (lambertian > 0.0) {
        vec3 viewDir = normalize(-position.xyz);
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        spec = pow(specAngle, 2);
    }

    vec3 ambient = street_light_ambient;
    vec3 diffuse = lambertian * street_light_diffuse;
    vec3 specular = spec * street_light_specular;

    return ambient + diffuse + specular;
}

float attenuation() {
    vec3 light_pos = lampPos;
    vec4 distanceVec = vertex - vec4(light_pos, 1.0);
    float distance = length(distanceVec);
    return 1/(constantAttn + linearAttn * distance + quadraticAttn * pow(distance, 2.0));
}

void main(void) {
    vec3 light = lampLight(vertex, normalize(normal)) * attenuation();
    vec4 textureColour = texture(texMap, st);
    vec4 preFogColour = textureColour * vec4(light, 1.0);
    vec4 skyColour = vec4(0.5, 0.5, 0.5, 1.0);
    vec4 fogColour = mix(skyColour, preFogColour, visibility);
    fragColour = fogColour;
}