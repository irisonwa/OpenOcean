#version 430 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture;
layout(location = 3) in ivec4 bone_ids;
layout(location = 4) in vec4 bone_weights;
layout(location = 5) in mat4 instance_trans; // transform of mesh instance
layout(location = 9) in float texture_depth;
layout(location = 10) in uint drawID;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 TexCoords;
layout(location = 3) out float tDepth;
layout(location = 4) flat out uint drawid;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 bones[200];

void main() {
  vec4 totalPos = vec4(0.0);
  vec3 totalNormal = vec3(0.0);
  int cnt = 0;
  for (int i = 0; i < 4; i++) {
    if (bone_ids[i] == -1) // ignore unbound bones
      continue;
    cnt++;
    mat4 bone = bones[bone_ids[i]];
    vec4 trans = bone * vec4(vertex_position, 1.0);
    totalPos += bone_weights[i] * trans;

    vec3 worldNormal = mat3(transpose(inverse(bone))) * vertex_normal;
    totalNormal += worldNormal * bone_weights[i];
  }

  if (cnt == 0) {
    totalPos = vec4(vertex_position, 0.0);
    totalNormal = vertex_normal;
  }
  FragPos = (instance_trans * totalPos).xyz;
  Normal = vec3(normalize(instance_trans * vec4(totalNormal, 0.0)));
  TexCoords = vertex_texture;
  tDepth = texture_depth;
  drawid = drawID;
  gl_Position = proj * view * instance_trans * totalPos;
}
