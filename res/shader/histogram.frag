#version 330

out vec4 frag_color;

smooth in vec2 tex_coord;

uniform float histogram[256];

void main (void)
{
    float c = step(histogram[int(round(tex_coord.x * 255.0))], tex_coord.y) + 0.66;
    frag_color = vec4(c, c, c, 1.0);
}
