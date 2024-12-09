// Uses instance transforms given on CPU
#version 460 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture;
layout(location = 3) in ivec4 bone_ids;
layout(location = 4) in vec4 bone_weights;
layout(location = 5) in mat4 instance_trans; // transform of mesh instance
layout(location = 9) in float texture_depth;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 TexCoords;
layout(location = 3) out float tDepth;
layout(location = 4) flat out uint drawID;

uniform mat4 view;
uniform mat4 proj;

void main() {
  drawID = uint(gl_DrawID); // to count variants
  vec4 totalPos = vec4(vertex_position, 1.0);
  FragPos = vec3(instance_trans * vec4(vertex_position, 1.0));
  Normal = mat3(transpose(inverse(instance_trans))) * vertex_normal;
  TexCoords = vertex_texture;
  tDepth = texture_depth;
  gl_Position = proj * view * instance_trans * totalPos;
}
