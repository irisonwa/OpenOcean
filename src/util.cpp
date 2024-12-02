#include "util.h"

namespace Util {
vec3 UP = vec3(0.f, 1.f, 0.f);
vec3 FORWARD = vec3(0.f, 0.f, -1.f);
vec3 RIGHT = vec3(1.f, 0.f, 0.f);
vec3 X = vec3(1.f, 0.f, 0.f);
vec3 Y = vec3(0.f, 1.f, 0.f);
vec3 Z = vec3(0.f, 0.f, 1.f);
std::random_device rand_dev;
std::mt19937 mt_gen(rand_dev());

std::string readFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Failed to open file " << path << std::endl;
        return "";
    }

    std::string text, line;
    while (getline(file, line)) {
        text += line + "\n";
    }

    file.close();
    text.append("\0");
    return text;
}

// Wrap a value between min and max. If val is greater than max, it will wraparound to min and begin climbing from there, and vice versa.
float wrap(float val, float min, float max) {
    return fmod(min + (val - min), max - min);
}

vec3 wrapV(vec3 val, vec3 min, vec3 max) {
    return vec3(
        wrap(val.x, min.x, max.x),
        wrap(val.y, min.y, max.y),
        wrap(val.z, min.z, max.z));
}

// Clamp a value between a minimum and maximum so that val cannot be greater than max or smaller than min.
float clamp(float val, float min, float max) {
    if (val < min) val = min;
    if (val > max) val = max;
    return val;
}

vec3 clampV(vec3 val, vec3 min, vec3 max) {
    return vec3(
        clamp(val.x, min.x, max.x),
        clamp(val.y, min.y, max.y),
        clamp(val.z, min.z, max.z));
}

// Linearly interpolate between `a` and `b` at time factor `t`. `t` should be mapped to 0-1
float lerp(float a, float b, float t) {
    if (t > 1) t = 1;
    if (t < 0) t = 0;
    return (a * (1 - t)) + b * t;
}

// Lerp the individual components of a vector
vec3 lerpV(vec3 a, vec3 b, float dt) {
    return vec3(
        lerp(a.x, b.x, dt),
        lerp(a.y, b.y, dt),
        lerp(a.z, b.z, dt));
}

// Generate a random number in the range (`lo`, `hi`).
int random(int lo, int hi) {
    std::uniform_int_distribution<> d(lo, hi);
    return d(mt_gen);
}

// Generate a vector with random components in the range (`lo`, `hi`).
vec3 randomv(int lo, int hi) {
    std::uniform_int_distribution<> d(lo, hi);
    return vec3(
        d(mt_gen),
        d(mt_gen),
        d(mt_gen)
    );
}

// Map a value `v` from input range `[inLow, inHigh]` to output range `[outLow, outHigh]`
// https://stackoverflow.com/a/5732390
float mapRange(float v, float inLow, float inHigh, float outLow, float outHigh) {
    float slope = (outHigh - outLow) / (inHigh - inLow);
    return outLow + slope * (v - inLow);
}

// Compute the squared distance between `a` and `b`. Use `glm::distance` for actual distance
float sqDist(vec3 a, vec3 b) {
    vec3 diff = a - b;
    return dot(diff, diff);
}

// Convert degrees to radians
float d2r(float val) { return glm::radians(val); }

// Convert radians to degrees
float r2d(float val) { return glm::degrees(val); }

// Convert an angle in degrees to a vec3. The angle will use the y-axis as the floor; i.e., the y-axis value will be 0.
vec3 angleToVec3(float angle) {
    return vec3(-cosf(d2r(angle)), 0, -sinf(d2r(angle)));
}

// Convert a vector to an angle in degrees. The angle will use the y-axis as the floor; i.e., the y-axis value will be ignored during the calculation.
float vec3ToAngle(vec3 v) {
    return r2d(atan2(v.y, v.x));
}

// Compare float values.
// Returns 0 if both floats are "equal" (difference is less than some epsilon)
// Returns -1 if `a` is less than `b`
// Returns 1 if `a` is greater than `b`
int compareFloat(float a, float b) {
    if (std::abs(a - b) < MIN_FLOAT_DIFF)
        return 0;
    else if (a < b)
        return -1;
    else
        return 1;
}

// Decompose a mat4 into a vec3 scale, quat rotation, vec3 translation, vec3 skew, and vec4 perspective
std::tuple<vec3, quat, vec3, vec3, vec4> decomposeMat4(mat4& mat) {
    vec3 scale;
    quat rotation;
    vec3 translation;
    vec3 skew;
    vec4 perspective;
    decompose(mat, scale, rotation, translation, skew, perspective);
    return {scale, rotation, translation, skew, perspective};
}

// Get the translation vector from a mat4
vec3 getTranslation(mat4& mat) {
    vec3 trans;
    std::tie(std::ignore, std::ignore, trans, std::ignore, std::ignore) = decomposeMat4(mat);
    return trans;
}

aiMatrix4x4 GLMtoAI(const mat4& mat) {
    aiMatrix4x4 p = aiMatrix4x4();
    p.a1 = mat[0][0];
    p.a2 = mat[0][1];
    p.a3 = mat[0][2];
    p.a4 = mat[0][3];
    p.b1 = mat[1][0];
    p.b2 = mat[1][1];
    p.b3 = mat[1][2];
    p.b4 = mat[1][3];
    p.c1 = mat[2][0];
    p.c2 = mat[2][1];
    p.c3 = mat[2][2];
    p.c4 = mat[2][3];
    p.d1 = mat[3][0];
    p.d2 = mat[3][1];
    p.d3 = mat[3][2];
    p.d4 = mat[3][3];
    return p;
}

mat4 aiToGLM(const aiMatrix4x4* from) {
    mat4 to;
    to[0][0] = (GLfloat)from->a1;
    to[0][1] = (GLfloat)from->b1;
    to[0][2] = (GLfloat)from->c1;
    to[0][3] = (GLfloat)from->d1;
    to[1][0] = (GLfloat)from->a2;
    to[1][1] = (GLfloat)from->b2;
    to[1][2] = (GLfloat)from->c2;
    to[1][3] = (GLfloat)from->d2;
    to[2][0] = (GLfloat)from->a3;
    to[2][1] = (GLfloat)from->b3;
    to[2][2] = (GLfloat)from->c3;
    to[2][3] = (GLfloat)from->d3;
    to[3][0] = (GLfloat)from->a4;
    to[3][1] = (GLfloat)from->b4;
    to[3][2] = (GLfloat)from->c4;
    to[3][3] = (GLfloat)from->d4;
    return to;
}

vec3 aiToGLM(aiVector3D* from) {
    return vec3(
        from->x,
        from->y,
        from->z
    );
}

// Create a matrix at position `from` looking in the direction `to`. Uses the global up direction (0, 1, 0)
mat4 lookTowards(vec3 from, vec3 to) {
    mat4 mat = translate(mat4(1), from);
    to = normalize(to);
    return mat * inverse(lookAt(from, from + to, vec3(0, 1, 0)));
}

// Create a matrix at position `from` looking in the direction `at`.
mat4 lookTowards(vec3 from, vec3 to, vec3 up) {
    mat4 mat = translate(mat4(1), from);
    to = normalize(to);
    return mat * inverse(lookAt(from, from + to, up));
}

void printVec3(vec3 v) {
    printf("(%f, %f, %f)\n", v.x, v.y, v.z);
}

void printVec2(vec2 v) {
    printf("(%f, %f)\n", v.x, v.y);
}

void printMat4(mat4 m) {
    printf("\n");
    printf("[%.2f][%.2f][%.2f][%.2f]\n", m[0][0], m[1][0], m[2][0], m[3][0]);
    printf("[%.2f][%.2f][%.2f][%.2f]\n", m[0][1], m[1][1], m[2][1], m[3][1]);
    printf("[%.2f][%.2f][%.2f][%.2f]\n", m[0][2], m[1][2], m[2][2], m[3][2]);
    printf("[%.2f][%.2f][%.2f][%.2f]\n", m[0][3], m[1][3], m[2][3], m[3][3]);
}

void printMat4(aiMatrix4x4 m) {
    printMat4(aiToGLM(&m));
}

void printList(std::vector<int> vs) {
    printf("[");
    for (int i = 0; i < vs.size(); ++i) {
        printf("%d", vs[i]);
        if (i != vs.size() - 1)
            printf(", ");
        
    }
    printf("]\n");
}

void printList(std::vector<float> vs) {
    printf("[");
    for (int i = 0; i < vs.size(); ++i) {
        printf("%.2f", vs[i]);
        if (i != vs.size() - 1)
            printf(", ");
        
    }
    printf("]\n");
}
}  // namespace Util