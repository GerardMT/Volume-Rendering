#version 330

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 norm;

// Noperspective necessary because we want coordinates to be screen intrepolated, no prespective interpolated
noperspective out vec2 pos_tex;

smooth out vec3 start_world;

uniform mat4 model;
uniform mat4 view_projection;

void main()
{
    vec4 world = model * vec4(vert, 1.0);
    gl_Position = view_projection * world;

    // Texture space (0.0 - 1.0), used to get the end wordl position in the fragment shader
    pos_tex = gl_Position.xy / gl_Position.w * 0.5 + 0.5;

    // Start world position
    start_world = world.xyz;
}
