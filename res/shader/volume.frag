#version 330

const float FLOAT_MAX = 3.402823466e+38;

out vec4 frag_color;

// Ray end (1st pass)
noperspective in vec2 pos_tex;
uniform sampler2D end_texture;

// Ray start
smooth in vec3 start_world; // Start

// Noise
uniform sampler2D noise_texture;
uniform float noise_step_size_factor;
uniform vec2 pixel_size;

// LUT
uniform sampler1D lut_texture;

// Volume
uniform sampler3D volume_texture;
uniform float step_size;
uniform float texture_max_size;
uniform float steps_factor;

// Lighting
uniform vec3 light_pos;
const vec3 light_inten = vec3(1.0);

// Blinn–Phong
uniform float k_a = 0.05;
uniform float k_d = 1.0;
uniform float k_s = 0.5;
uniform float shininess = 128.0;

// Shadows
uniform float steps_factor_shadow;
uniform bool shadows;

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

float offset;

/*
 * Correct the sampled density using the LUT.
 * v: sampled density.
 * return the corrected density, from 0 to 1.
 */

float correct(float v)
{
    return texture(lut_texture, v).a;
}

/*
 * Sample the volume.
 * pos: position to sample.
 * return the volume sampled, from 0 to 1.
 */
float volume(vec3 pos)
{
    return texture(volume_texture, -pos * 0.5 + 0.5).r;
}

/*
 * Return the color using the LUT.
 * density: from 0 to 1.
 * return the volume color.
 */
vec3 color(float density)
{
    return texture(lut_texture, density).rgb;
}

/*
 * Computes the box face line intersection. Line can never be coplanar with any box face.
 * pos: line point.
 * l: line unit vector.
 * face: box face index.
 * retun the line d parameter if intersects or FLOAT_MAX if it does not intersect.
 */
float plane_point_intersection(vec3 pos, vec3 l, int face)
{
    float den = dot(l, cube_norm[face]);
    if (den > 0.0) {
        return dot((cube_point[face] - pos), cube_norm[face]) / den;
    } else {
        return FLOAT_MAX;
    }
}

/*
 * Given the position and light vector, compute the shadow.
 * Uses a hardcoded 2x2x2 box centered at 0 as the ray end point.
 * pos: volume position, line point.
 * l: line unit vector.
 * return 1 if it is completly in shadow or 0 is there is no shadow.
 */
float shadow(vec3 pos, vec3 l)
{
    // Compute the end point by computing the cube light vector intersaction
    float d0 = plane_point_intersection(pos, l, 0);
    float d1 = plane_point_intersection(pos, l, 1);
    float d2 = plane_point_intersection(pos, l, 2);
    float d3 = plane_point_intersection(pos, l, 3);
    float d4 = plane_point_intersection(pos, l, 4);
    float d5 = plane_point_intersection(pos, l, 5);

    float d = min(min(min(min(min(d0, d1), d2), d3), d4), d5);
    vec3 end = pos + d * l;

    // Add the random offset
    pos = start_world + normalize(end - pos) * offset;

    vec3 start_end = end - pos;

    // Compute the number of steps required (without early alpha termination)
    int n_steps = int(ceil(length(start_end) / step_size * steps_factor_shadow));
    // Step size
    vec3 step = start_end / n_steps;

    float opacity_corr_ratio = float(n_steps) / texture_max_size;
    float alpha = 0.0;
    for (int i = 0; i < n_steps; ++i) {
        // Alpha accomulation with oppacity correction
        alpha = alpha + pow((1.0 - alpha), opacity_corr_ratio) * correct(volume(pos));

        // Early alpha termination
        if (alpha >= 1.0) {
            break;
        }

        pos += step;
    }

    return alpha;
}

/*
 * Compute the lighting given a position. Using the Blinn–Phong reflection model.
 * pos: volume position.
 * v: view unit vector.
 * denstiy: volume density at position pos.
 * reutrn the RGB lighting.
 */
vec3 lighting(vec3 pos, vec3 v, float density)
{
    vec3 n = -normalize(vec3(volume(pos + vec3(step_size, 0.0, 0.0)) - volume(pos - vec3(step_size, 0.0, 0.0)),
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

    // Compute a random offset for this fragment
    offset = texture(noise_texture, pos_tex / (pixel_size * textureSize(noise_texture, 0))).r * (step_size * noise_step_size_factor);
    // Add the random offset
    vec3 start = start_world + normalize(end - start_world) * offset;

    vec3 start_end = end - start;

    // Compute the number of steps required (without early alpha termination)
    int n_steps = int(ceil(length(start_end) / step_size * steps_factor));
    // Step size
    vec3 step = start_end / n_steps;

    // Phong
    vec3 v = normalize(-start_end);

    float opacity_corr_ratio = float(n_steps) / texture_max_size;
    vec3 pos = start;
    frag_color = vec4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < n_steps; ++i) {
        float density = volume(pos);

        // Alpha accomulation with oppacity correction
        float alpha = pow((1.0 - frag_color.a), opacity_corr_ratio);
        frag_color.rgb = frag_color.rgb + alpha * lighting(pos, v, density);
        frag_color.a = frag_color.a + alpha * correct(density);

        // Early alpha termination
        if (frag_color.a >= 1.0) {
            break;
        }

        pos += step;
    }
}
