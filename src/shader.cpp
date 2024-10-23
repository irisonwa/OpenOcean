#include "shader.h"

void Shader::AddShader(GLuint ShaderProgram, const char* pShaderText,
                       GLenum ShaderType) {
    // Create a shader object
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }
    // Bind the source code to the shader, this happens before compilation
    glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderText, NULL);
    // Compile the shader and check for errors
    glCompileShader(ShaderObj);
    GLint success;
    // Check for shader related errors using glGetShaderiv
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType,
                InfoLog);
        exit(1);
    }
    // Attach the compiled shader object to the program object
    glAttachShader(ShaderProgram, ShaderObj);
}

GLuint Shader::CompileShaders(const char* pVS, const char* pFS) {
    // Start the process of setting up our shaders by creating a program ID
    // Note: we will link all the shaders together into this ID
    GLuint shaderProgramID = glCreateProgram();
    if (shaderProgramID == 0) {
        std::cerr << "Error creating shader program..." << std::endl;
        std::cerr << "Press enter/return to exit..." << std::endl;
        std::cin.get();
        exit(1);
    }

    // Create two shader objects, one for the vertex, and one for the fragment shader
    std::string pvs = Util::readFile(pVS);
    std::string pfs = Util::readFile(pFS);
    AddShader(shaderProgramID, pvs.c_str(), GL_VERTEX_SHADER);
    AddShader(shaderProgramID, pfs.c_str(), GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = {'\0'};
    // After compiling all shader objects and attaching them to the program, we
    // can finally link it
    glLinkProgram(shaderProgramID);
    // check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
        std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
        std::cerr << "Press enter/return to exit..." << std::endl;
        std::cin.get();
        exit(1);
    }

    // program has been successfully linked but needs to be validated to check
    // whether the program can execute given the current pipeline state
    glValidateProgram(shaderProgramID);
    // check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
        std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
        std::cerr << "Press enter/return to exit..." << std::endl;
        std::cin.get();
        exit(1);
    }

    // Note: this program will stay in effect for all draw calls until you
    // replace it with another or explicitly disable its use
    return shaderProgramID;
}