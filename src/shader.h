#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <GLM/vec2.hpp>
#include <GLM/vec3.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "util.h"

class Shader {
   public:
    GLuint ID = 0;
    std::string name;
    Shader() {}
    Shader(std::string shader_name, const char* vertex_shader_path,
           const char* fragment_shader_path) {
        name = shader_name;
        ID = CompileShaders(vertex_shader_path, fragment_shader_path);
    }

    Shader(std::string shader_name, const char* compute_shader_path) {
        name = shader_name;
        ID = CompileComputeShader(compute_shader_path);
    }

    void AddShader(GLuint ShaderProgram, const char* pShaderText,
                   GLenum ShaderType);
    GLuint CompileShaders(const char* pVS, const char* pFS);
    GLuint CompileComputeShader(const char* pCS);

    // activate the shader
    // ------------------------------------------------------------------------
    void use() { glUseProgram(ID); }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setVec2(const std::string& name, vec2 value) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
    }
    void setVec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    void setVec3(const std::string& name, vec3 v) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), v.x, v.y, v.z);
    }
    void setVec4(const std::string& name, vec4 v) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), v.x, v.y, v.z, v.w);
    }
    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                           &mat[0][0]);
    }
};

#endif /* SHADER_H */
