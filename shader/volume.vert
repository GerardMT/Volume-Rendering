#version 330

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 norm;

noperspective out vec2 tex_pos;

uniform mat4 model;
uniform mat4 view_projection;

void main()
{
    mat4 mvp = view_projection * model;

    gl_Position = mvp * vec4(vert, 1.0);

    tex_pos = gl_Position.xy / gl_Position.w * 0.5 + 0.5;
}
