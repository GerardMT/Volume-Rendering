#version 330

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 norm;

uniform mat4 model;
uniform mat4 view_projection;

smooth out vec3 pos_world;

void main()
{
    vec4 world = model * vec4(vert, 1.0);
    pos_world = world.xyz;

    gl_Position = view_projection * world;
}
