#version 460 core
layout (location = 0) in vec3 vertex_position;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 proj;

void main()
{
    TexCoords = vertex_position;
    vec4 pos = proj * view * vec4(vertex_position, 1.0);
    gl_Position = pos.xyww;
}

