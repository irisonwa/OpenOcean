#include "util.h"

// const used to convert degrees into radians
#define TWO_PI 2.0f * 3.14159265358979323846
#define ONE_DEG_IN_RAD (2.0f * 3.14159265358979323846) / 360.0f  // 0.017444444
#define ONE_RAD_IN_DEG 57.2957795

namespace Util {
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
        return t > 0 ? 
            clamp(a + t, a, b) :
            clamp(a + t, b, a);
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

    // Point a matrix at `pos` in the direction of `dir`
    mat4 lookTowards(vec3 pos, vec3 dir) {
        mat4 mat = translate(mat4(1), pos);
        return mat * inverse(lookAt(pos, pos + dir, vec3(0, 1, 0)));
    }
}