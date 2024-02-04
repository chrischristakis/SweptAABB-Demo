#pragma once

#include <string>
#include <vector>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
private:

	using UniformMap = std::map<std::string, GLint>;
	UniformMap uniformLocations_;

public:

	unsigned int ID;
	Shader() = default;
	Shader(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath = "");

	void Create(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath = "");
	Shader& Use();

	unsigned int CreateShader(GLenum type, const std::string &path);

	GLint GetUniformLocation(const std::string &name);

	void SetMatrix4f(const std::string &name, glm::mat4 &val);
	void SetVec2f(const std::string &name, glm::vec2 &val);
	void SetVec3f(const std::string &name, glm::vec3 &val);
	void SetVec4f(const std::string &name, glm::vec4 &val);
	void SetInt(const std::string &name, int val);
	void SetFloat(const std::string &name, float val);
};