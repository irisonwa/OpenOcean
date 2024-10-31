#version 430 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture;
layout(location = 3) in mat4 instance_trans;
layout(location = 7) in float texture_depth;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float tDepth;

uniform mat4 view;
uniform mat4 proj;

void main() {
  FragPos = vec3(instance_trans * vec4(vertex_position, 1.0));
  Normal = mat3(transpose(inverse(instance_trans))) * vertex_normal;
  TexCoords = vertex_texture;
  tDepth = texture_depth;
  gl_Position = proj * view * vec4(FragPos, 1.0);
}
