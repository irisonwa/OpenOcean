#version 330
layout(location = 0) in vec3 aPos;       // position vertex
layout(location = 1) in vec3 aNormal;    // normal vertex
layout(location = 2) in vec2 aTexCoords; // texture vertex
layout(location = 3) in mat4 aInstance;  // instance matrix

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
  FragPos = vec3(aInstance * vec4(aPos, 1.0));
  Normal = mat3(transpose(inverse(aInstance))) * aNormal;
  TexCoords = aTexCoords;
  gl_Position = projection * view * vec4(FragPos, 1.0);
}