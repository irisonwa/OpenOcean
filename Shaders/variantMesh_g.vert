// Uses instance transforms taken from boids.comp (GPU)
#version 460 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture;
layout(location = 3) in ivec4 bone_ids;
layout(location = 4) in vec4 bone_weights;
layout(location = 5) in mat4 instance_transs; // transform of mesh instance
layout(location = 9) in float texture_depth;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 TexCoords;
layout(location = 3) out float tDepth;
layout(location = 4) flat out uint drawID;

struct BoneInfo {
  mat4 offsetMatrix;                       // inverse bind matrix for the bone                                     # 4 * 4 * 4 = 64
  mat4 currentTransformation;              // current transformation of the bone                                   # 4 * 4 * 4 = 64
  int children[16];                        // ids of children bones                                                # 4 * 16 = 64
  int ID;                                  // bone id                                                              # 4
  // vec2 bi_padding;                      // padding                                                              # 4 * 2 = 8
  // float bi_padding2;                    // padding                                                              # 4
};

layout (std430, binding = 1) buffer readonly BoneInfos {
    BoneInfo infos[];
};

layout (std430, binding = 2) buffer readonly BoneOffsets {
    int boffsets[];
};

layout (std430, binding = 6) buffer readonly BTransforms {
    mat4 instance_trans[];
};

uniform mat4 view;
uniform mat4 proj;

void main() {
  drawID = uint(gl_DrawID); // to count variants
  uint iid = uint(gl_BaseInstance + gl_InstanceID); // instance id
  // drawID = uint(gl_BaseInstance + gl_InstanceID); // to count instances

  vec4 totalPos = vec4(0.0);
  vec3 totalNormal = vec3(0.0);
  int cnt = 0; // number of bones
  for (int i = 0; i < 4; i++) {
    if (bone_ids[i] == -1) // ignore unbound bones
      continue;
    cnt++;
    mat4 bone = infos[bone_ids[i] + boffsets[drawID]].currentTransformation;
    totalPos += bone_weights[i] * bone * vec4(vertex_position, 1.0);

    vec3 worldNormal = mat3(transpose(inverse(bone))) * vertex_normal;
    totalNormal += worldNormal * bone_weights[i];
  }

  if (cnt == 0) {
    // if no bones, animate as if it's static
    totalPos = vec4(vertex_position, 1.0);
    FragPos = vec3(instance_trans[iid] * vec4(vertex_position, 1.0));
    Normal = mat3(transpose(inverse(instance_trans[iid]))) * vertex_normal;
  } else {
    FragPos = vec3(instance_trans[iid] * totalPos);
    Normal = vec3(normalize(instance_trans[iid] * vec4(totalNormal, 0.0)));
  }

  TexCoords = vertex_texture;
  tDepth = texture_depth;
  gl_Position = proj * view * instance_trans[iid] * totalPos;
}
