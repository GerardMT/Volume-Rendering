#version 330

out vec4 frag_color;

smooth in vec2 quad_coord;

const float blur = 0.05;
const float radius = 1.0;

void main (void)
{
    float col = smoothstep(radius, radius - blur, abs(length(quad_coord)));

    frag_color = vec4(col, 0.0, 0.0, col);
}
