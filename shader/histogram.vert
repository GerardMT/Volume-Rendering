#version 330

layout (location = 0) in vec2 vert;

smooth out vec2 tex_coord;

void main()
{
    tex_coord = vert * 0.5 + 0.5;

    gl_Position = vec4(vert, 0.0, 1.0);
}
