#version 330

layout (location = 0) in vec3 vert;

uniform mat4 model;
uniform mat4 view_projection;

void main()
{
    gl_Position = view_projection * model * vec4(vert, 1.0);
}
