#version 330 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

// struct Light {
//     vec3 position;
//
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// Directional Light
// struct Light {
//     vec3 direction;
//
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// Point Light
// struct Light {
//     vec3 position;
//
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
//
//     float constant;
//     float linear;
//     float quadratic;
// };
//
// Flashlight
struct Light {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
    vec3 diffuseMap = texture(material.diffuse, TexCoords).rgb;
    vec3 specularMap = texture(material.specular, TexCoords).rgb;

    vec3 lightDir = normalize(light.position - FragPos);

    // Ambient
    vec3 ambient = light.ambient * diffuseMap;

    // Diffuse
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseMap;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * specularMap;

    // Spotlight
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);
    diffuse *= intensity;
    specular *= intensity;

    // Attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
                light.quadratic * (distance * distance));

    // ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0f);
}
