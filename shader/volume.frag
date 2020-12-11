#version 330

const float FLOAT_MAX = 3.402823466e+38;

out vec4 frag_color;

noperspective in vec2 pos_tex;
uniform sampler2D end_texture;

smooth in vec3 start_world; // Start

uniform sampler2D noise_texture;
const float noise_step_size_factor = 1.0;

uniform sampler1D lut_texture;

uniform sampler3D volume_texture;
uniform float step_size;
uniform float texture_max_size;

uniform vec2 pixel_size;

uniform float steps_factor;
uniform float steps_factor_shadow;
uniform bool shadows;

uniform vec3 light_pos;
const vec3 light_inten = vec3(0.5);

const float k_a = 0.05;
const float k_d = 1.0;
const float k_s = 1.0;
const float shininess = 128.0;


const vec3 cube_point[6] = vec3[]( // Hardcoded
    vec3( 1.0,  0.0,  0.0),
    vec3(-1.0,  0.0,  0.0),
    vec3( 0.0,  1.0,  0.0),
    vec3( 0.0, -1.0,  0.0),
    vec3( 0.0,  0.0,  1.0),
    vec3( 0.0,  0.0, -1.0)
);

const vec3 cube_norm[6] = vec3[]( // Hardcoded
    vec3( 1.0,  0.0,  0.0),
    vec3(-1.0,  0.0,  0.0),
    vec3( 0.0,  1.0,  0.0),
    vec3( 0.0, -1.0,  0.0),
    vec3( 0.0,  0.0,  1.0),
    vec3( 0.0,  0.0, -1.0)
);

float correct(float v)
{
    return texture(lut_texture, v).a;
}

float volume(vec3 pos)
{
    return texture(volume_texture, -pos * 0.5 + 0.5).r;
}

vec3 color(float density)
{
    return texture(lut_texture, density).rgb;
}

float plane_point_intersection(vec3 pos, vec3 l, int face)
{
    float den = dot(l, cube_norm[face]);
    if (den > 0.0) {
        return dot((cube_point[face] - pos), cube_norm[face]) / den;
    } else {
        return FLOAT_MAX;
    }
}

float shadow(vec3 pos, vec3 l)
{
    float d0 = plane_point_intersection(pos, l, 0);
    float d1 = plane_point_intersection(pos, l, 1);
    float d2 = plane_point_intersection(pos, l, 2);
    float d3 = plane_point_intersection(pos, l, 3);
    float d4 = plane_point_intersection(pos, l, 4);
    float d5 = plane_point_intersection(pos, l, 5);

    float d = min(min(min(min(min(d0, d1), d2), d3), d4), d5);
    vec3 end = pos + d * l;

    float offset = texture(noise_texture, pos_tex / (pixel_size * textureSize(noise_texture, 0))).r * (step_size * noise_step_size_factor);
    pos = start_world + normalize(end - pos) * offset;

    vec3 start_end = end - pos;

    int n_steps = int(ceil(length(start_end) / step_size * steps_factor_shadow));
    n_steps = min(max(n_steps, 0), 1000); // DEBUG

    vec3 step = start_end / n_steps;

    float alpha = 0.0;
    int i;
    for (i = 0; i < n_steps; ++i) {
        alpha = alpha + pow((1.0 - alpha), float(n_steps) / texture_max_size) * correct(volume(pos));

        if (alpha >= 1.0) {
            alpha = 1.0; // F Compiler bug
            break;
        }

        pos += step;
    }

    return alpha;
}

vec3 lighting(vec3 pos, vec3 v, float density)
{
    vec3 n = normalize(vec3(volume(pos + vec3(step_size, 0.0, 0.0)) - volume(pos - vec3(step_size, 0.0, 0.0)),
                            volume(pos + vec3(0.0, step_size, 0.0)) - volume(pos - vec3(0.0, step_size, 0.0)),
                            volume(pos + vec3(0.0, 0.0, step_size)) - volume(pos - vec3(0.0, 0.0, step_size))));
    vec3 l = normalize(light_pos - pos);
    vec3 h = normalize(l + v);

    vec3 c = color(density);

    vec3 ambi = k_a * c;
    vec3 diff = k_d * max(0.0, dot(l, n)) * light_inten * c;
    vec3 spec = k_s * pow(max(0.0, dot(n, h)), shininess) * light_inten;

    float s = 0.0;
    if (shadows) {
        s = shadow(pos, l);
    }
    return ambi + ((diff + spec) * (1.0 - s));
}

void main (void)
{
    vec3 end = texture(end_texture, pos_tex).rgb;

    float offset = texture(noise_texture, pos_tex / (pixel_size * textureSize(noise_texture, 0))).r * (step_size * noise_step_size_factor);
    vec3 start = start_world + normalize(end - start_world) * offset;

    vec3 start_end = end - start;

    int n_steps = int(ceil(length(start_end) / step_size * steps_factor));
    n_steps = min(max(n_steps, 0), 1000); // DEBUG

    vec3 step = start_end / n_steps;

    frag_color = vec4(0.0, 0.0, 0.0, 0.0);

    // Phong
    vec3 v = normalize(-start_end);
    // ----

    vec3 pos = start;
    for (int i = 0; i < n_steps; ++i) {
        float density = volume(pos);

        float alpha = pow((1.0 - frag_color.a), float(n_steps) / texture_max_size);
        frag_color.rgb = frag_color.rgb + alpha * lighting(pos, v, density);
        frag_color.a = frag_color.a + alpha * correct(density);

        if (frag_color.a >= 1.0) {
            frag_color.a = 1.0; // F Compiler bug
            break;
        }

        pos += step;
    }
}
