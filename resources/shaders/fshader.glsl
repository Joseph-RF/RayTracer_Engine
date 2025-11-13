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

// SHADOWS
// -------
uniform samplerCube[POINT_LIGHTS_CAPACITY] depth_maps;
uniform float far_plane;
uniform bool use_pcf;
vec3 sample_offset_directions[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);
int samples = 20;

in vec3 normal;
in vec3 frag_pos;

out vec4 FragColor; // Fragment shader only requires a single output variable

vec3 calculateDirLight(DirLight light, vec3 norm, vec3 view_dir);
vec3 calculatePointLight(PointLight light, vec3 norm, vec3 fragment_pos, vec3 view_dir, int point_light_index);

void main() {
    // Remember to NORMALISE vectors we'll be doing maths with
    vec3 norm     = normalize(normal);
    vec3 view_dir = normalize(viewer_pos - frag_pos);

    vec3 colour_output = vec3(0.0);

    // 1. Directional lights
    // colour_output += calculateDirLight(dir_light, norm, view_dir);

    // 2. Point lights
    for (int i = 0; i < point_lights_number; ++i) {
        colour_output += calculatePointLight(point_lights[i], norm, frag_pos, view_dir, i);
    }

    FragColor = vec4(colour_output, 1.0f); // Output must be vec4
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
    vec3 diffuse         = diffuse_factor * light.colour * light.diffuse * colour;

    // Specular Light
    // -------------------------------------
    // Using Blinn-Phong, using halfway vector between viewing direction and light direction
    vec3 halfway_dir = normalize(view_dir + light_direction);

    // Power index represents how "focused" the specular component is on the face
    // The higher it is, the more concentrated, the lower it is, the more spread out
    // NOTE: material here is the global variable material found at this fragment
    float specular_factor = pow(max(dot(norm, halfway_dir), 0.0), shininess);
    vec3 specular         = specular_factor * light.colour * light.specular * colour;

    return ambient + diffuse + specular;
};

vec3 calculatePointLight(PointLight light, vec3 norm, vec3 fragment_pos, vec3 view_dir, int point_light_index) {

    // Normalise vectors we're going to use
    // Want to direction of light to be from fragment TO light
    vec3 light_direction = normalize(light.position - fragment_pos);

    // Attenuation
    // -------------------------------------
    float dist        = length(fragment_pos - light.position);
    float attenuation = light.constant + light.linear * dist + light.quadratic * dist * dist;
    attenuation       = 1.0 / attenuation;

    // Ambient
    // -------------------------------------
    vec3 ambient = light.ambient * light.colour * colour;

    // Diffuse
    // -------------------------------------
    float diffuse_factor = max(dot(light_direction, norm), 0.0);
    vec3 diffuse         = diffuse_factor * light.diffuse * light.colour * colour;

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
    if (dot(light_direction, norm) > 0.0) {
        specular_factor = pow(max(dot(norm, halfway_dir), 0.0), shininess);
    }

    vec3 specular = specular_factor * light.specular * light.colour * colour;

    // Account for attenuation
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // SHADOWS
    // ---------
    vec3 frag_to_light = fragment_pos - light.position;
    float current_depth = length(frag_to_light); // Current linear depth as the length between the fragment and light position
    float closest_depth;
    float shadow;
    float bias;

    if (use_pcf) {
        bias = 0.15;
        float view_distance = length(viewer_pos - fragment_pos);
        float disk_radius = 0.05;

        for(int i = 0; i < samples; ++i) {
            closest_depth = texture(depth_maps[point_light_index], frag_to_light + sample_offset_directions[i] * disk_radius).r;
            closest_depth *= far_plane;   // undo mapping [0;1]
            if(current_depth - bias > closest_depth) {
                shadow += 1.0;
            }
        }
        shadow /= float(samples);
    } else {
        bias = 0.05;
        closest_depth = texture(depth_maps[point_light_index], frag_to_light).r;
        // Transform from range between [0,1]. Re-transform back to original value
        closest_depth *= far_plane;
        // test for shadows
        shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;
    }

    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);

    vec3 total = vec3(0.0);
    total += ambient;
    total += diffuse;
    total += specular;

    return total;
};