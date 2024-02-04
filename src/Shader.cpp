#include <sstream>
#include <fstream>
#include <iostream>
#include "Shader.h"

Shader::Shader(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath) {
    Create(vsPath, fsPath, gsPath);
}

unsigned int Shader::CreateShader(GLenum type, const std::string &path) {
    std::ifstream stream(path);
    std::string source;

    if (stream) {
        std::stringstream shaderStream;
        shaderStream << stream.rdbuf(); // Read buffer into the string stream
        stream.close();
        source = shaderStream.str();
    }
    else {
        std::cerr << "ERROR: Cannot read shader file: " << path << std::endl;
        return 0;
    }

    const char *src_c_str = source.c_str();

    // Attach shaders
    unsigned int shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &src_c_str, NULL);
    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cerr << "ERROR: " << path << ": " << infoLog << std::endl;
        return 0;
    }

    return shaderID;
}

void Shader::Create(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath) {
    unsigned int vShader = CreateShader(GL_VERTEX_SHADER, vsPath);
    unsigned int fShader = CreateShader(GL_FRAGMENT_SHADER, fsPath);
    unsigned int gShader = 0;

    if (!gsPath.empty())
        gShader = CreateShader(GL_GEOMETRY_SHADER, gsPath);

    unsigned int programID = glCreateProgram();

    glAttachShader(programID, vShader);
    glAttachShader(programID, fShader);
    if (!gsPath.empty())
        glAttachShader(programID, gShader);

    glLinkProgram(programID);
    int success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cout << "Error linking program: " << infoLog << std::endl;
        return;
    }

    this->ID = programID;

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    if (!gsPath.empty())
        glDeleteShader(gShader);
}

Shader& Shader::Use() {
    glUseProgram(this->ID);
    return *this;
}

GLint Shader::GetUniformLocation(const std::string &name) {
    UniformMap::iterator location = uniformLocations_.find(name);

    // Insert location into map if it doesn't exist
    if (location == uniformLocations_.end()) {
        GLint location = glGetUniformLocation(this->ID, name.c_str());
        uniformLocations_.insert({ name, location});
        return location;
    }

    return location->second;
}

void Shader::SetMatrix4f(const std::string &name, glm::mat4 &val) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &val[0][0]);
}

void Shader::SetVec3f(const std::string &name, glm::vec3 &val) {
    glUniform3fv(GetUniformLocation(name), 1, &val[0]);
}

void Shader::SetVec2f(const std::string &name, glm::vec2 &val) {
    glUniform2fv(GetUniformLocation(name), 1, &val[0]);
}

void Shader::SetVec4f(const std::string &name, glm::vec4 &val) {
    glUniform4fv(GetUniformLocation(name), 1, &val[0]);
}

void Shader::SetInt(const std::string &name, int val) {
    glUniform1i(GetUniformLocation(name), val);
}

void Shader::SetFloat(const std::string &name, float val) {
    glUniform1f(GetUniformLocation(name), val);
}