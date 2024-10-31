#version 430 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture;
layout(location = 3) in ivec4 bone_ids;
layout(location = 4) in vec4 bone_weights;
layout(location = 5) in mat4 instance_trans; // transform of mesh instance

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 bones[200];

void main() {
  vec4 localPos = vec4(0.0);
  vec3 localNormal = vec3(0.0);

  for (int i = 0; i < 4; i++) {
    if (bone_ids[i] == -1) // ignore unbound bones
      continue;

    mat4 bone = bones[bone_ids[i]];
    vec4 trans = (bone * bone_weights[i]) * vec4(vertex_position, 1.0);
    localPos += trans;

    vec3 worldNormal = mat3(transpose(inverse(bone))) * vertex_normal;
    localNormal += worldNormal * bone_weights[i];
  }

  FragPos = vec3(instance_trans * localPos);
  Normal = localNormal;
  TexCoords = vertex_texture;
  gl_Position = proj * view * instance_trans * localPos;
}
