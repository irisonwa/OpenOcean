#include "util.h"

// const used to convert degrees into radians
#define TWO_PI 2.0f * 3.14159265358979323846
#define ONE_DEG_IN_RAD (2.0f * 3.14159265358979323846) / 360.0f  // 0.017444444
#define ONE_RAD_IN_DEG 57.2957795

namespace Util {
    vec3 UP = vec3(0.f, 1.f, 0.f);
    vec3 FORWARD = vec3(0.f, 0.f, -1.f);
    vec3 RIGHT = vec3(1.f, 0.f, 0.f);
    vec3 X = vec3(1.f, 0.f, 0.f);
    vec3 Y = vec3(0.f, 1.f, 0.f);
    vec3 Z = vec3(0.f, 0.f, 1.f);


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

    // Wrap a value between min and max. If val is greater than max, it will
    // wraparound to min and begin climbing from there, and vice versa.
    float wrap(float val, float min, float max) {
        return fmod(min + (val - min), max - min);
    }

    // Clamp a value between a minimum and maximum so that val cannot be greater
    // than max or smaller than min.
    float clamp(float val, float min, float max) {
        if (val < min) val = min;
        if (val > max) val = max;
        return val;
    }

    float lerp(float a, float b, float t) {
        if (t > 1) t = 1;
        if (t < 0) t = 0;
        return (a * (1 - t)) + b * t;
    }
    
    vec3 lerpV(vec3 a, vec3 b, float dt) {
        return vec3(
            lerp(a.x, b.x, dt),
            lerp(a.y, b.y, dt),
            lerp(a.z, b.z, dt)
        );
    }

    // Convert degrees to radians
    float deg2Rad(float val) { return val * ONE_DEG_IN_RAD; }

    // Convert radians to degrees
    float rad2Deg(float val) { return val * ONE_RAD_IN_DEG; }

    // Convert an angle in degrees to a vec3. The angle will use the y-axis as the floor; i.e., the y-axis value will be 0.
    vec3 angleToVec3(float angle) {
        return vec3(-cosf(deg2Rad(angle)), 0, -sinf(deg2Rad(angle)));
    }

    // Convert a vector to an angle in degrees. The angle will use the y-axis as the floor; i.e., the y-axis value will be ignored during the calculation.
    float vec3ToAngle(vec3 v) {
        return rad2Deg(atan2(v.y, v.x));
    }

    FloatComp compareFloat(float a, float b) {
        if (std::abs(a - b) < MIN_FLOAT_DIFF) return FloatComp::EQ;
        else if (a < b) return FloatComp::LT;
        else return FloatComp::GT;
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
        aiMatrix4x4* p = new aiMatrix4x4();
        p->a1 = mat[0][0];
        p->a2 = mat[0][1];
        p->a3 = mat[0][2];
        p->a4 = mat[0][3];
        p->b1 = mat[1][0];
        p->b2 = mat[1][1];
        p->b3 = mat[1][2];
        p->b4 = mat[1][3];
        p->c1 = mat[2][0];
        p->c2 = mat[2][1];
        p->c3 = mat[2][2];
        p->c4 = mat[2][3];
        p->d1 = mat[3][0];
        p->d2 = mat[3][1];
        p->d3 = mat[3][2];
        p->d4 = mat[3][3];
        return *p;
    }

    mat4 aiToGLM(aiMatrix4x4* from) {
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
    };

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

    void printMat4(mat4 m) {
        printf("\n");
        printf("[%.2f][%.2f][%.2f][%.2f]\n", m[0][0], m[1][0], m[2][0], m[3][0]);
        printf("[%.2f][%.2f][%.2f][%.2f]\n", m[0][1], m[1][1], m[2][1], m[3][1]);
        printf("[%.2f][%.2f][%.2f][%.2f]\n", m[0][2], m[1][2], m[2][2], m[3][2]);
        printf("[%.2f][%.2f][%.2f][%.2f]\n", m[0][3], m[1][3], m[2][3], m[3][3]);
    }
}