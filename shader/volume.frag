#version 330

out vec4 frag_color;

noperspective in vec2 tex_pos;

uniform sampler2D end_texture;

void main (void) {
    frag_color = vec4(texture(end_texture, tex_pos).rgb, 1.0);
    //frag_color = vec4(tex_pos, 0.0, 1.0);
}
