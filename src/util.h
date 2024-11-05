#ifndef UTIL_H
#define UTIL_H

#include <cmath>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

#include <assimp/scene.h>        // collects data
#include <assimp/postprocess.h>  // various extra operations

#include "sm.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define PROJDIR "../"
#define MODELDIR(m) PROJDIR "Models/" + m.substr(0, m.find(".")) + "/"
#define MIN_FLOAT_DIFF 0.00000001f

using namespace glm;
// using namespace SM;

// Enum values for float comparison, for ease of use
enum FloatComp {
    EQ,
    LT,
    GT
};

namespace Util {
    extern vec3 UP;  // World up
    extern vec3 FORWARD;  // World forward
    extern vec3 RIGHT;  // World right
    extern vec3 X;  // Value of 1 on x axis
    extern vec3 Y;  // Value of 1 on y axis
    extern vec3 Z;  // Value of 1 on z axis

    extern std::string readFile(const char* path);
    extern float wrap(float val, float min, float max);
    extern float clamp(float val, float min, float max);
    extern float deg2Rad(float val);
    extern float rad2Deg(float val);
    extern aiMatrix4x4 GLMtoAI(const mat4& mat);
    extern mat4 aiToGLM(aiMatrix4x4* from);
    extern std::tuple<vec3, quat, vec3, vec3, vec4> decomposeMat4(mat4& mat);
    extern vec3 getTranslation(mat4& mat);
    extern vec3 angleToVec3(float angle);
    extern float vec3ToAngle(vec3 v);
    extern float lerp(float a, float b, float dt);
    extern vec3 lerpV(vec3 a, vec3 b, float dt);
    extern mat4 lookTowards(vec3 pos, vec3 to);
    extern mat4 lookTowards(vec3 pos, vec3 to, vec3 up);
    extern void printVec3(vec3 v);
    extern void printMat4(mat4 m);
    extern FloatComp compareFloat(float a, float b);
};

#endif  // UTIL_H
