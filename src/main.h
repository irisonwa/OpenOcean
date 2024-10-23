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
#define MESH_SPIRE "cone.gltf"
#define MESH_LAMP "lamppost.gltf"
#define MESH_MON1 "monument1.gltf"
#define MESH_MON1GUY "monument_1_zombie.obj"
#define MESH_MONKEY "monkey.obj"
#define MESH_ROAD "ground.obj"
#define MESH_TRAM "tram.gltf"
#define MESH_TREE "tree.gltf"
#define MESH_GPO "gpo.gltf"
#define MESH_BUILDING1 "building1.obj"
#define MESH_BUILDING2 "building2.obj"
#define MESH_BUILDING3 "building3.obj"
#define MESH_BUILDING4 "building4.obj"
#define MESH_BUS "bus.gltf"
#define MESH_RUBBLE "rubble.gltf"
#define BMESH_ENEMY "enemy.gltf"

#define TEST_CUBE "test_cube.obj"
#define TEST_PRISM "prism.glb"
#define TEST_FISH "fsh.obj"
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
std::vector<Mesh*> meshes;
std::vector<vec3> translations, scales;
int spread = 10;

std::vector<std::string> cubemap_faces = {
    PROJDIR "Models/bskybox/right.jpg",
    PROJDIR "Models/bskybox/left.jpg",
    PROJDIR "Models/bskybox/top.jpg",
    PROJDIR "Models/bskybox/bottom.jpg",
    PROJDIR "Models/bskybox/front.jpg",
    PROJDIR "Models/bskybox/back.jpg",
};

#endif /* MAIN_H */
