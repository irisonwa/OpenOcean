#ifndef MAIN_H
#define MAIN_H

#define NOMINMAX
#include <limits>

// Windows includes (For Time, IO, etc.)
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include <stddef.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>  // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

// Assimp includes
#include <assimp/cimport.h>      // scene importer
#include <assimp/postprocess.h>  // various extra operations
#include <assimp/scene.h>        // collects data

// Project includes
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "sm.h"
#include "util.h"
#include "cubemap.h"
#include "lighting.h"
#include "boid.h"
// #include "bonemesh.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#pragma region MESH_NAMES
#define MESH_WALL "wall.gltf"
#define MESH_LONGWALL "wall_4x1.gltf"
#define TEST_CUBE "test_cube.obj"
#define TEST_INST "inst_cube.obj"
#define TEST_SPEC "spec_cube.obj"
#define TEST_PRISM "prism.glb"
#define TEST_FISHA "fsh.obj"
#define TEST_FISHB "fsh2.obj"
#define TEST_BOID "boid.obj"
#define TEST_GROUND "test_ground.obj"
#pragma endregion


std::map<std::string, Shader*> shaders;
const char* vert_main = PROJDIR "Shaders/main.vert";
const char* frag_main = PROJDIR "Shaders/main.frag";
const char* vert_sb = PROJDIR "Shaders/cubemap.vert";
const char* frag_sb = PROJDIR "Shaders/cubemap.frag";
const char* vert_bmesh = PROJDIR "Shaders/boneMesh.vert";
const char* frag_bmesh = PROJDIR "Shaders/boneMesh.frag";
const char* vert_wire = PROJDIR "Shaders/wireframe.vert";
const char* frag_wire = PROJDIR "Shaders/wireframe.frag";
const char* vert_blank = PROJDIR "Shaders/blank.vert";
const char* frag_blank = PROJDIR "Shaders/blank.frag";

Cubemap* cubemap;
Camera camera = Camera();
Lighting *baseLight;

std::deque<Boid*> boids;

vec3 flashlightCoords = vec3(-10000);
vec3 flashlightDir = vec3(0, -1, 0);
std::map<std::string, Mesh*> meshes;
std::vector<vec3> translations, scales;
std::vector<int> mat_idxs;
std::vector<float> depths;
int spread = 10;

std::vector<std::string> cubemap_faces = {
    PROJDIR "Models/bskybox/right.jpg",
    PROJDIR "Models/bskybox/left.jpg",
    PROJDIR "Models/bskybox/top.jpg",
    PROJDIR "Models/bskybox/bottom.jpg",
    PROJDIR "Models/bskybox/front.jpg",
    PROJDIR "Models/bskybox/back.jpg",
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
