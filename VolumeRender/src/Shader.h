#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

class CShader
{
public:
	CShader()
		: isLinked(false)
	{
		program = glCreateProgram();
	};

	~CShader()
	{
		glDeleteProgram(program);
	};

	void loadShader(const std::string &strShaderFile)
	{
		std::string type = strShaderFile.substr(strShaderFile.length() - 9, 4);
		GLenum shaderType;
		if (type == "geom")
			shaderType = GL_GEOMETRY_SHADER;
		else if (type == "comp")
			shaderType = GL_COMPUTE_SHADER;
		else if (type == "frag")
			shaderType = GL_FRAGMENT_SHADER;
		else if (type == "vert")
			shaderType = GL_VERTEX_SHADER;
		else if (type == "tess")
			shaderType = GL_TESS_EVALUATION_SHADER;
		else if (type == "ctrl")
			shaderType = GL_TESS_CONTROL_SHADER;
		else
			throw std::runtime_error("bad filename");

		loadShader(shaderType, strShaderFile);
	}

	void loadShader(GLenum eShaderType, const std::string &strShaderFile)
	{
		std::ifstream in(strShaderFile);
		if (!in.good())
		{
			// Visual Studio: Set Project properties->Debugging->Working Directory to $(TargetDir)
			throw std::runtime_error("File not found.");
		}
		std::stringstream buffer;
		buffer << in.rdbuf();

		addShader(eShaderType, buffer.str());
	}

	void addShader(GLenum eShaderType, const std::string &shaderCode)
	{
		GLuint shaderId = glCreateShader(eShaderType);
		const char *strFileData = shaderCode.c_str();
		glShaderSource(shaderId, 1, &strFileData, NULL);

		glCompileShader(shaderId);

		GLint status;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint infoLogLength;
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar *strInfoLog = new GLchar[infoLogLength + 1];
			glGetShaderInfoLog(shaderId, infoLogLength, NULL, strInfoLog);

			const char *strShaderType = NULL;
			switch (eShaderType)
			{
			case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
			case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
			case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
			case GL_TESS_CONTROL_SHADER: strShaderType = "tess control"; break;
			case GL_TESS_EVALUATION_SHADER: strShaderType = "tess eval"; break;
			case GL_COMPUTE_SHADER: strShaderType = "compute"; break;
			}

			fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
			delete[] strInfoLog;
		}

		shaderList.push_back(shaderId);
	};

	void link(GLsizei countCaptured = 0, const char *captured[] = nullptr)
	{
		isLinked = true;

		for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
			glAttachShader(program, shaderList[iLoop]);

		if (countCaptured)
			glTransformFeedbackVaryings(program, countCaptured, captured, GL_SEPARATE_ATTRIBS);

		glLinkProgram(program);

		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint infoLogLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar *strInfoLog = new GLchar[infoLogLength + 1];
			glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
			fprintf(stderr, "Linker failure: %s\n", strInfoLog);
			delete[] strInfoLog;
		}
		else
			std::cout << "Shader program " << program << " linked successfully" << std::endl;

		for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
			glDetachShader(program, shaderList[iLoop]);

		std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
		shaderList.clear();
	}

	void use()
	{
		if (!isLinked)
			std::cout << "Program " << program << " wasn't linked" << std::endl;
		glUseProgram(program);
		usedProgram = program;
	}

	GLint uniform(const std::string& uniformName)
	{
		if (usedProgram != program)
		{
			std::cout << "WARNING: Program not bound!" << std::endl;
		}

		auto it = uniMap.find(uniformName);
		if (it == uniMap.end())
		{
			GLint result = glGetUniformLocation(program, uniformName.c_str());
			//std::cout << "Found uniform " << uniformName << " in program " << program << " at " << result << std::endl;
			uniMap[uniformName] = result;
			return result;
		}

		return it->second;
	}

	void uniform(const std::string& uniformName, GLint value) { glUniform1i(uniform(uniformName), value); }
	void uniform(const std::string& uniformName, GLuint value) { glUniform1ui(uniform(uniformName), unsigned int(value)); }
	void uniform(const std::string& uniformName, GLfloat value) { glUniform1f(uniform(uniformName), value); }
	void uniform(const std::string& uniformName, bool value) { uniform(uniformName, GLuint(value)); }

	void uniform(const std::string& uniformName, const glm::ivec2& value) { glUniform2iv(uniform(uniformName), 1, value_ptr(value)); }
	void uniform(const std::string& uniformName, const glm::ivec3& value) { glUniform3iv(uniform(uniformName), 1, value_ptr(value)); }
	void uniform(const std::string& uniformName, const glm::vec3& value) { glUniform3fv(uniform(uniformName), 1, value_ptr(value)); }
	void uniform(const std::string& uniformName, const glm::vec4& value) { glUniform4fv(uniform(uniformName), 1, value_ptr(value)); }

	void uniform(const std::string& uniformName, const glm::mat4& value, GLboolean transpose = GL_FALSE)
	{
		glUniformMatrix4fv(uniform(uniformName), 1, transpose, value_ptr(value));
	}

	// Prevent implicit casts.
	template <typename T> void uniform(const std::string&, T) = delete;

	void unUse()
	{
		usedProgram = 0;
		glUseProgram(0);
	}

private:
	static GLuint usedProgram;
	GLuint program;
	bool isLinked;
	std::vector<GLuint> shaderList;
	std::map<std::string, GLint> uniMap;
};

GLuint CShader::usedProgram = 0;