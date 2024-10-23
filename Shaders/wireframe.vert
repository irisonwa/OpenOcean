#version 330 
layout (location = 0) in vec3 att;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main() {
    gl_Position = proj * view * model * vec4(att, 1.0);
}

