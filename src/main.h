#ifndef MAIN_H
#define MAIN_H

#define NOMINMAX
#define GLM_ENABLE_EXPERIMENTAL

// C includes
#include <windows.h>
#include <mmsystem.h>

// C++ includes
#include <limits>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

// Assimp includes
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// ImGui includes
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glut.h"
#include "imgui/imgui_impl_opengl3.h"

// Project includes
#include "camera.h"
#include "mesh.h"
#include "staticmesh.h"
#include "bonemesh.h"
#include "variantmesh.h"
#include "shader.h"
#include "sm.h"
#include "util.h"
#include "flock.h"
#include "lighting.h"
#include "boid.h"
#include "player.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

/* Meshes */
// Plants
#define MESH_KELP_ANIM "kelp.gltf"  // Kelp model (animated)
#define MESH_ANEMONE "anemone.obj"  // Anemone model (static) [THIRD PARTY, see README.md]
// Fish
#define MESH_SHARK_ANIM "shark.gltf"               // Generic shark model (animated)
#define MESH_WHALE_SHARK_ANIM "shark_whale.gltf"   // Whale shark model (animated)
#define MESH_WHITE_SHARK_ANIM "shark_white.gltf"   // White shark model (animated)
#define MESH_BLUE_SHARK_ANIM "shark_blue.gltf"     // Blue shark model (animated)
#define MESH_WHALE_ANIM "whale.gltf"               // Whale model (animated)
#define MESH_DOLPHIN_ANIM "dolphin.gltf"           // Dolphin model (animated)
#define MESH_PLANKTON_ANIM "plankton.gltf"         // Plankton model (animated)
#define MESH_THREADFIN_ANIM "fish_threadfin.gltf"  // Threadfin model (animated)
#define MESH_MARLIN_ANIM "fish_marlin.gltf"        // Marlin model (animated)
#define MESH_SPEARFISH_ANIM "fish_spear.gltf"      // Spearfish model (animated)
#define MESH_CLOWNFISH_ANIM "fish_clown.gltf"      // Clownfish model (animated)
#define MESH_HERRING_ANIM "fish_herring.gltf"      // Herring model (animated)
// Other
#define MESH_SUN "sun.obj"
#define MESH_PLAYER_ANIM "sub.gltf"       // Player submarine (animated)
#define MESH_BEACON "beacon.gltf"         // Light beacon (static)
#define MESH_TERRAIN "terrain.gltf"       // Terrain model (static)
#define MESH_BEACH "beach.gltf"           // Beach model (static)
#define MESH_BEACH_ITEM "beach_item.obj"  // Beach items model (static) [THIRD PARTY, see README.md]

const char* vert_smesh = PROJDIR "Shaders/staticMesh.vert";
const char* frag_smesh = PROJDIR "Shaders/staticMesh.frag";
const char* vert_bmesh = PROJDIR "Shaders/boneMesh.vert";
const char* frag_bmesh = PROJDIR "Shaders/boneMesh.frag";
const char* vert_vmesh_cpu = PROJDIR "Shaders/variantMesh_c.vert";
const char* vert_vmesh_gpu = PROJDIR "Shaders/variantMesh_g.vert";
const char* frag_vmesh = PROJDIR "Shaders/variantMesh.frag";
const char* vert_sb = PROJDIR "Shaders/cubemap.vert";
const char* frag_sb = PROJDIR "Shaders/cubemap.frag";
const char* vert_blank = PROJDIR "Shaders/blank.vert";
const char* frag_blank = PROJDIR "Shaders/blank.frag";

Lighting *staticLight, *boneLight, *variantLight;
std::vector<Boid*> boids;
Flock* flock;
Player* player;
VariantMesh *flockVariants, *skinnedVariants, *staticVariants;
std::vector<mat4> skvMats, stvMats;
Box* updateBox;

bool showBoids = true;
bool showGround = false;
bool showLevelBounds = true;
bool showUpdateBounds = true;
bool useHeightBackground = true;
vec3 flashlightCoords = vec3(-10000);
vec3 flashlightDir = vec3(0, -1, 0);
vec3 sunPos = vec3(0, 500, 0);
vec3 sunDir = Util::UP * -1.f;
float seaLevel = 100;
std::map<std::string, Shader*> shaders;
std::map<std::string, StaticMesh*> smeshes;
std::map<std::string, BoneMesh*> bmeshes;
std::vector<vec3> translations, scales;
std::vector<mat4> transm;
std::vector<int> mat_idxs;
std::vector<float> depths;

std::vector<mat4> beaconLightMats;
std::vector<vec3> beaconLightPos = {
    vec3(-5.517541, -171.772491, -184.788651) * vec3(1, 0.8, 1),
    vec3(-246.655991, -156.900360, -225.508698) * vec3(1, 0.8, 1),
    vec3(-168.904816, -167.625061, 100.703346) * vec3(1, 0.8, 1),
    vec3(-122.003487, -167.601807, 238.243576) * vec3(1, 0.8, 1),
    vec3(-16.958982, -162.666428, 216.117126) * vec3(1, 0.8, 1),
    vec3(147.293274, -255.615295, 262.316681) * vec3(1, 0.8, 1),
    vec3(24.694633, -217.530853, 304.397736) * vec3(1, 0.8, 1),
    vec3(269.658295, -201.544113, 132.361252) * vec3(1, 0.8, 1),
    vec3(61.435417, -181.549606, 172.717377) * vec3(1, 0.8, 1),
    vec3(125.427429, -163.743774, -309.644012) * vec3(1, 0.8, 1),
    vec3(14.141843, -161.406906, -226.288147) * vec3(1, 0.8, 1),
    vec3(-34.764236, -221.480225, -6.967876) * vec3(1, 0.8, 1),
    vec3(16.821505, -213.663513, -3.734689) * vec3(1, 0.8, 1),
    vec3(310.439575, -194.521057, 79.667366) * vec3(1, 0.8, 1),
    vec3(101.371223, -113.593376, -59.377941),
    vec3(73.735527, -129.348282, 54.185513),
    vec3(-283.396637, -116.780930, -36.396130),
    vec3(-451.899231, -130.391678, -7.988328),
    vec3(-111.995651, -133.852020, -376.756104),
    vec3(104.884323, -149.077881, 345.610077),
};

std::vector<mat4> anemoneMats, kelpMats;
std::vector<vec3> anemonePos = {
    vec3(-5.517541, -171.772491, -184.788651),
    vec3(-246.655991, -156.900360, -225.508698),
    vec3(-168.904816, -167.625061, 100.703346),
    vec3(-122.003487, -167.601807, 238.243576),
    vec3(-16.958982, -162.666428, 216.117126),
    vec3(147.293274, -255.615295, 262.316681),
    vec3(24.694633, -217.530853, 304.397736),
    vec3(269.658295, -201.544113, 132.361252),
    vec3(61.435417, -181.549606, 172.717377),
    vec3(125.427429, -163.743774, -309.644012),
    vec3(14.141843, -161.406906, -226.288147),
    vec3(-34.764236, -221.480225, -6.967876),
    vec3(16.821505, -213.663513, -3.734689),
    vec3(310.439575, -194.521057, 79.667366),
    vec3(282.533051, -183.193207, -126.183655),
    vec3(230.126678, -174.246231, -106.679169),
};

std::vector<mat4> islandMats = {
    translate(scale(mat4(1), vec3(22)), vec3(13, -6, 13)),
    translate(scale(mat4(1), vec3(15)), vec3(-184.215073, -152.724846, -139.598846) / 15.f),
    translate(scale(mat4(1), vec3(15)), vec3(291.893799, -140.834908, -227.146240) / 15.f),
    translate(scale(mat4(1), vec3(15)), vec3(-354.657928, -163.792442, 226.254578) / 15.f),
};

// error/debug message callback, from https://www.khronos.org/opengl/wiki/Debug_Output#Examples
void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

#endif /* MAIN_H */
