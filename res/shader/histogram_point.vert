#version 330

layout (location = 0) in vec2 vert;
layout (location = 1) in vec2 trans;

uniform float ratio;

uniform float size;

smooth out vec2 quad_coord;

void main()
{
    quad_coord = vert;

    vec2 scale = vec2(1.0, ratio) * size;
    gl_Position = vec4((vert * scale) + trans * 2.0 - 1.0, 0.0, 1.0);
}
