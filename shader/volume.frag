#version 330

out vec4 frag_color;

noperspective in vec2 pos_tex;

smooth in vec3 start_world; // Start

uniform sampler2D end_texture;

uniform sampler3D volume_texture;

uniform sampler2D noise_texture;

uniform float step_size;

uniform vec2 pixel_size;

uniform float max_density;

const float threshold = 0.1;
const float density = 0.05;
const float steps = 2.0;

void main (void) {
    vec3 end_world = texture(end_texture, pos_tex).rgb;

    float offset = texture(noise_texture, pos_tex / (pixel_size * textureSize(noise_texture, 0))).r * (step_size * 0.1);
    end_world += normalize(start_world - end_world) * offset;

    vec3 end_start_world = start_world - end_world;

    int n_steps = int(ceil(length(end_start_world) / step_size * steps));
    n_steps = max(n_steps, 0); // DEBUG

    vec3 step_world = end_start_world / n_steps;

    frag_color = vec4(0.0, 0.0, 0.0, 1.0);

    vec3 pos_world = end_world;
    int i = 0;
    while (i < n_steps && frag_color.a > 0.0) {
        float alpha = texture(volume_texture, -pos_world * 0.5 + 0.5).r / max_density;
        if (alpha < threshold) {
           alpha = 0.0;
        }
        alpha *= density;

        vec3 color = vec3(0.0);
        frag_color.rgb = color + (1.0 - alpha) * frag_color.rgb;
        frag_color.a *= (1.0 - alpha);

        ++i;
        pos_world += step_world;
    }

    frag_color.a = 1.0 - frag_color.a;

    if (frag_color.a >= 1.0) {
        //frag_color = vec4(1.0, 0.0, 0.0, 1.0);
    } else if (frag_color.a == 0.0) {
        //frag_color = vec4(0.0, 1.0, 0.0, 1.0);
    }

    //float c = end_world;
    //frag_color = vec4(end_world, 1.0);
}
