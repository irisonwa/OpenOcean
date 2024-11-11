#version 460 core

precision highp float;

// bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad
// bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad
// bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad bad
// i only have 11 meshes in the variant so this is technically ok, but this is
// limiting and stupid using regular uniforms might have been better than this
layout(binding = 0) uniform highp sampler2DArray diffSampler1;
layout(binding = 1) uniform highp sampler2DArray mtlSampler1;
layout(binding = 2) uniform highp sampler2DArray diffSampler2;
layout(binding = 3) uniform highp sampler2DArray mtlSampler2;
layout(binding = 4) uniform highp sampler2DArray diffSampler3;
layout(binding = 5) uniform highp sampler2DArray mtlSampler3;
layout(binding = 6) uniform highp sampler2DArray diffSampler4;
layout(binding = 7) uniform highp sampler2DArray mtlSampler4;
layout(binding = 8) uniform highp sampler2DArray diffSampler5;
layout(binding = 9) uniform highp sampler2DArray mtlSampler5;
layout(binding = 10) uniform highp sampler2DArray diffSampler6;
layout(binding = 11) uniform highp sampler2DArray mtlSampler6;
layout(binding = 12) uniform highp sampler2DArray diffSampler7;
layout(binding = 13) uniform highp sampler2DArray mtlSampler7;
layout(binding = 14) uniform highp sampler2DArray diffSampler8;
layout(binding = 15) uniform highp sampler2DArray mtlSampler8;
layout(binding = 16) uniform highp sampler2DArray diffSampler9;
layout(binding = 17) uniform highp sampler2DArray mtlSampler9;
layout(binding = 18) uniform highp sampler2DArray diffSampler10;
layout(binding = 19) uniform highp sampler2DArray mtlSampler10;
layout(binding = 20) uniform highp sampler2DArray diffSampler11;
layout(binding = 21) uniform highp sampler2DArray mtlSampler11;

layout(binding = 30) uniform highp sampler2DArray samplers[][2];

struct Material {
  highp sampler2DArray diffuse;
  highp sampler2DArray specular;
  float shininess;
};

struct DirLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct SpotLight {
  vec3 position;
  vec3 direction;
  float cutOff;
  float outerCutOff;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

#define NR_DIR_LIGHTS 100
#define NR_POINT_LIGHTS 100
#define NR_SPOT_LIGHTS 100

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;
layout(location = 3) in float tDepth;
layout(location = 4) flat in uint drawID;

layout(location = 0) out vec4 FragColour;

uniform vec3 viewPos;
uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform Material material;
uniform int nPointLights = 10;
uniform int nSpotLights = 10;
uniform int nDirLights = 10;
uniform vec4 fogColour;
uniform vec2 fogBounds;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 getLightFromSamplers(float diff, float spec, float attenuation,
                          float intensity, vec3 light_ambient,
                          vec3 light_diffuse, vec3 light_specular);

void main() {
  // properties
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);

  // directional lights
  vec3 result = vec3(0.0);
  for (int i = 0; i < nDirLights; i++)
    result += CalcDirLight(dirLights[i], norm, viewDir);

  // point lights
  for (int i = 0; i < nPointLights; i++)
    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

  // spot lights
  for (int i = 0; i < nSpotLights; i++)
    result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);

  // Fog
  float fog_lower_bound = fogBounds.x;
  float fog_upper_bound = fogBounds.y;
  float dist = length(viewPos - FragPos);
  float fog_scale =
      (fog_upper_bound - dist) / (fog_upper_bound - fog_lower_bound);
  fog_scale = clamp(fog_scale, 0.0, 1.0);

  FragColour = mix(fogColour, vec4(result, 1.0), fog_scale);
  // if (drawID % 2 == 0)
  //   FragColour = vec4(Normal, 1.0);
  // else
  // FragColour = vec4(result, 1.0);
}

// calculate the light value and use the correct sampler to apply it to textures
vec3 getLightFromSamplers(float diff, float spec, float attenuation,
                          float intensity, vec3 light_ambient,
                          vec3 light_diffuse, vec3 light_specular) {
  vec3 ambient = vec3(0);
  vec3 diffuse = vec3(0);
  vec3 specular = vec3(0);
  switch (drawID) {
  case 0:
    ambient =
        light_ambient * vec3(texture(diffSampler1, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler1, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler1, vec3(TexCoords, tDepth)));
    break;
  case 1:
    ambient =
        light_ambient * vec3(texture(diffSampler2, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler2, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler2, vec3(TexCoords, tDepth)));
    break;
  case 2:
    ambient =
        light_ambient * vec3(texture(diffSampler3, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler3, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler3, vec3(TexCoords, tDepth)));
    break;
  case 3:
    ambient =
        light_ambient * vec3(texture(diffSampler4, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler4, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler4, vec3(TexCoords, tDepth)));
    break;
  case 4:
    ambient =
        light_ambient * vec3(texture(diffSampler5, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler5, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler5, vec3(TexCoords, tDepth)));
    break;
  case 5:
    ambient =
        light_ambient * vec3(texture(diffSampler6, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler6, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler6, vec3(TexCoords, tDepth)));
    break;
  case 6:
    ambient =
        light_ambient * vec3(texture(diffSampler7, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler7, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler7, vec3(TexCoords, tDepth)));
    break;
  case 7:
    ambient =
        light_ambient * vec3(texture(diffSampler8, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler8, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler8, vec3(TexCoords, tDepth)));
    break;
  case 8:
    ambient =
        light_ambient * vec3(texture(diffSampler9, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler9, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler9, vec3(TexCoords, tDepth)));
    break;
  case 9:
    ambient =
        light_ambient * vec3(texture(diffSampler10, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler10, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler10, vec3(TexCoords, tDepth)));
    break;
  case 10:
    ambient =
        light_ambient * vec3(texture(diffSampler11, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler11, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler11, vec3(TexCoords, tDepth)));
    break;
  default:
    ambient =
        light_ambient * vec3(texture(diffSampler1, vec3(TexCoords, tDepth)));
    diffuse = light_diffuse * diff *
              vec3(texture(diffSampler1, vec3(TexCoords, tDepth)));
    specular = light_specular * spec *
               vec3(texture(mtlSampler1, vec3(TexCoords, tDepth)));
    break;
  }
  ambient *= attenuation * intensity;
  diffuse *= attenuation * intensity;
  specular *= attenuation * intensity;
  return ambient + diffuse + specular;
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  return getLightFromSamplers(diff, spec, 1, 1, light.ambient, light.diffuse,
                              light.specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  // attenuation
  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance +
                             light.quadratic * (distance * distance));
  return getLightFromSamplers(diff, spec, attenuation, 1, light.ambient,
                              light.diffuse, light.specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance +
                             light.quadratic * (distance * distance));

  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  return getLightFromSamplers(diff, spec, attenuation, intensity, light.ambient,
                              light.diffuse, light.specular);
}
