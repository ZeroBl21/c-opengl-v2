#version 330 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

// Directional Light
struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Point Light
struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

// Spotlight
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outer_cutoff;

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

#define NR_POINT_LIGHTS 4
uniform PointLight point_lights[NR_POINT_LIGHTS];
uniform DirLight dir_light;
uniform SpotLight spot_light;

// function prototypes
vec3 calc_dir_light(DirLight light, vec3 normal, vec3 viewDir);
vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 viewDir);
vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 frag_pos, vec3 viewDir);

void main() {
    // properties
    vec3 norm = normalize(Normal);
    vec3 view_dir = normalize(viewPos - FragPos);

    // phase 1: directional lighting
    vec3 result = calc_dir_light(dir_light, norm, view_dir);
    // phase 2: point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        result += calc_point_light(point_lights[i], norm, FragPos, view_dir);
    // phase 3: spot light
    result += calc_spot_light(spot_light, norm, FragPos, view_dir);

    FragColor = vec4(result, 1.0);
}

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_dir) {
    vec3 diffuse_map = texture(material.diffuse, TexCoords).rgb;
    vec3 specular_map = texture(material.specular, TexCoords).rgb;

    vec3 light_dir = normalize(-light.direction);

    // Diffuse
    float diff = max(dot(normal, light_dir), 0.0);

    // Specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    // Results
    vec3 ambient = light.ambient * diffuse_map;
    vec3 diffuse = light.diffuse * diff * diffuse_map;
    vec3 specular = light.specular * spec * specular_map;

    return ambient + diffuse + specular;
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
    vec3 diffuse_map = texture(material.diffuse, TexCoords).rgb;
    vec3 specular_map = texture(material.specular, TexCoords).rgb;

    vec3 light_dir = normalize(light.position - frag_pos);

    // Diffuse
    float diff = max(dot(normal, light_dir), 0.0);

    // Specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Results
    vec3 ambient = light.ambient * diffuse_map;
    vec3 diffuse = light.diffuse * diff * diffuse_map;
    vec3 specular = light.specular * spec * specular_map;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
    vec3 diffuse_map = texture(material.diffuse, TexCoords).rgb;
    vec3 specular_map = texture(material.specular, TexCoords).rgb;

    vec3 light_dir = normalize(light.position - frag_pos);

    // Diffuse
    float diff = max(dot(normal, light_dir), 0.0);

    // Specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    // Attenuation
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight Intensity
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

    // Results
    vec3 ambient = light.ambient * diffuse_map;
    vec3 diffuse = light.diffuse * diff * diffuse_map;
    vec3 specular = light.specular * spec * specular_map;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}
