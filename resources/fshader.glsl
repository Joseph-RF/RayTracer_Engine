#version 330 core

uniform vec3 colour;
uniform float shininess;
uniform int point_lights_number;

struct DirLight {
    vec3 direction;

    vec3 colour;

    float ambient;
    float diffuse;
    float specular;
};
uniform DirLight dir_light;

struct PointLight {
    vec3 position;

    vec3 colour;

    float ambient;
    float diffuse;
    float specular;

    float constant;
    float linear;
    float quadratic;
};
#define POINT_LIGHTS_CAPACITY 16
uniform PointLight point_lights[POINT_LIGHTS_CAPACITY];

uniform vec3 viewer_pos;

in vec3 normal;
in vec3 frag_pos;

out vec4 FragColor; // Fragment shader only requires a single output variable

vec3 calculateDirLight(DirLight light, vec3 norm, vec3 view_dir);
vec3 calculatePointLight(PointLight light, vec3 norm, vec3 fragment_pos, vec3 view_dir);

void main() {
    // Remember to NORMALISE vectors we'll be doing maths with
    vec3 norm = normalize(normal);
    vec3 view_dir = normalize(viewer_pos - frag_pos);

    vec3 colour_output = vec3(0.0);

    // 1. Directional lights
    // colour_output += calculateDirLight(dir_light, norm, view_dir);

    // 2. Point lights
    for(int i = 0; i < point_lights_number; ++i) {
        colour_output += calculatePointLight(point_lights[i], norm, frag_pos, view_dir);
    }

    FragColor = vec4(colour_output, 1.0f);// Output must be vec4
};

vec3 calculateDirLight(DirLight light, vec3 norm, vec3 view_dir) {

    // Flip sign of light direction since we have been defining light direction as
    // fragment TO light. People prefer directional light to be calculated with
    // light direction defined as being FROM the light.
    vec3 light_direction = normalize(light.direction);

    // Ambient Light
    // -------------------------------------
    vec3 ambient = light.colour * light.ambient * colour;

    // Diffuse Light
    // -------------------------------------
    float diffuse_factor = max(dot(light_direction, norm), 0.0);
    vec3 diffuse = diffuse_factor * light.colour * light.diffuse * colour;

    // Specular Light
    // -------------------------------------
    // Using Blinn-Phong, using halfway vector between viewing direction and light direction
    vec3 halfway_dir = normalize(view_dir + light_direction);

    // Power index represents how "focused" the specular component is on the face
    // The higher it is, the more concentrated, the lower it is, the more spread out
    // NOTE: material here is the global variable material found at this fragment
    float specular_factor = pow(max(dot(norm, halfway_dir), 0.0), shininess);
    vec3 specular = specular_factor * light.colour * light.specular * colour;

    return ambient + diffuse + specular;
};

vec3 calculatePointLight(PointLight light, vec3 norm, vec3 fragment_pos, vec3 view_dir) {

    // Normalise vectors we're going to use
    // Want to direction of light to be from fragment TO light
    vec3 light_direction = normalize(light.position - fragment_pos);

    // Attenuation
    // -------------------------------------
    float dist = length(fragment_pos - light.position);
    float attenuation = light.constant + light.linear * dist
                        + light.quadratic * dist * dist;
    attenuation = 1.0 / attenuation;

    // Ambient
    // -------------------------------------
    vec3 ambient = light.ambient * light.colour * colour;

    // Diffuse
    // -------------------------------------
    float diffuse_factor = max(dot(light_direction, norm), 0.0);
    vec3 diffuse = diffuse_factor * light.diffuse * light.colour * colour;

    // Specular
    // -------------------------------------
    // Using Blinn-Phong, using halfway vector between viewing direction and light direction
    vec3 halfway_dir = normalize(view_dir + light_direction);

    // Power index represents how "focused" the specular component is on the face
    // The higher it is, the more concentrated, the lower it is, the more spread out
    // NOTE: material here is the global variable material found at this fragment
    float specular_factor = 0.0;

    // Below check prevents a specular components on fragments facing away from 
    // light source
    if(dot(light_direction, norm) > 0.0) {
        specular_factor = pow(max(dot(norm, halfway_dir), 0.0), shininess);
    }

    vec3 specular = specular_factor * light.specular * light.colour * colour;

    // Account for attenuation
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 total = vec3(0.0);
    total += ambient;
    total += diffuse;
    total += specular;

    return total;
};