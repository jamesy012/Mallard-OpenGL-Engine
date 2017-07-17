#include "Shader.h"

#include <GL\glew.h>
#include <stdio.h>

#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

static Shader* m_LastUsed = nullptr;

Shader::Shader() {
	m_Program = 0;
	for (int i = 0; i < SHADER_TYPES_SIZE; i++) {
		m_Shaders[i] = 0;
	}
	m_Linked = false;
}


Shader::~Shader() {
	//todo delete program
}

void Shader::setFromPath(ShaderTypes a_Type, const char * a_FilePath) {
	std::ifstream shaderFile(a_FilePath);
	std::stringstream fileBuffer;
	fileBuffer << shaderFile.rdbuf();//get text from file into buffer
	std::string shaderText = fileBuffer.str();

	if (shaderText.size() <= 5) {
		printf("shader loaded from file was empty %s\n", a_FilePath);
		return;
	}

	const GLchar* shaderCode = shaderText.c_str();

	createShader(a_Type, &shaderCode);
}

void Shader::setFromText(ShaderTypes a_Type, const char * a_ShaderText) {
	createShader(a_Type, &a_ShaderText);
}

void Shader::createSimpleShader(bool a_Textured) {
	std::string vertex =
		"#version 410													\n"
		"layout(location = 0) in vec4 position;							\n"
		"layout(location = 2) in vec2 texCoord;							\n"
		"																\n"
		"uniform mat4 projectionViewMatrix;								\n"
		"uniform mat4 model = mat4(1);									\n"
		"																\n"
		"out vec2 vTexCoord;											\n"
		"																\n"
		"void main() {													\n"
		"	gl_Position = projectionViewMatrix * model * position;		\n"
		"	vTexCoord = texCoord; 										\n"
		"}																";

	std::string fragment;

	if (a_Textured) {
		fragment = "#version 410\n"
			" "
			"in vec2 vTexCoord; "
			"out vec4 fragColor; "
			" "
			"uniform vec4 color = vec4(1,1,1,1); "
			"uniform sampler2D TexDiffuse1; "
			" "
			"void main() { "
			"	vec4 col = texture(TexDiffuse1, vTexCoord); "
			//"	fragColor = (col * color)*col.a; "
			//"	fragColor = vec4(col.xyz,1); "
			"	fragColor = col; "
			"} ";
	} else {
		fragment = "#version 410\n"
			" "
			"uniform vec4 color = vec4(1,1,1,1); "
			"out vec4 fragColor; "
			" "
			"void main() { "
			"	fragColor = color; "
			"} ";
	}

	setFromText(ShaderTypes::TYPE_VERTEX, vertex.c_str());
	setFromText(ShaderTypes::TYPE_FRAGMENT, fragment.c_str());
	linkShader();

}

void Shader::createProgram() {
	if (m_Program != 0) {
		printf("shader already had a program");
	}
	m_Program = glCreateProgram();
}

void Shader::linkShader() {
	if (m_Linked) {
		printf("Shader already linked\n");
		return;
	}

	if (m_Program == 0) {
		createProgram();
	}

	//create shader
	for (int i = 0; i < SHADER_TYPES_SIZE; i++) {
		if (m_Shaders[i] != 0) {
			glAttachShader(m_Program, m_Shaders[i]);
		}
	}
	glLinkProgram(m_Program);

	checkGlErrorProgram(GL_LINK_STATUS, m_Program, "LINKING_FAILED");

	for (int i = 0; i < SHADER_TYPES_SIZE; i++) {
		if (m_Shaders[i] != 0) {
			glDeleteShader(m_Shaders[i]);
			m_Shaders[i] = 0;
		}
	}

}

unsigned int Shader::getProgram() {
	return m_Program;
}

void Shader::use() {
	glUseProgram(m_Program);
	m_LastUsed = this;
}

Shader * Shader::getCurrentShader() {
	return m_LastUsed;
}

unsigned int Shader::getOpenglShaderType(ShaderTypes a_Type) {
	switch (a_Type) {
		case ShaderTypes::TYPE_VERTEX:
			return GL_VERTEX_SHADER;
		case ShaderTypes::TYPE_FRAGMENT:
			return GL_FRAGMENT_SHADER;
		default:
			return 0;
	}
}

void Shader::createShader(ShaderTypes a_Type, const char * const * a_Code) {
	unsigned int shaderIndex = glCreateShader(getOpenglShaderType(a_Type));
	glShaderSource(shaderIndex, 1, a_Code, NULL);
	glCompileShader(shaderIndex);

	checkGlErrorShader(GL_COMPILE_STATUS, shaderIndex, "COMPILATION_FAILED");

	m_Shaders[(int)a_Type] = shaderIndex;
}

bool Shader::checkGlErrorProgram(const int a_ErrorType, const unsigned int a_Program, const char * a_ErrorMessage) {
	GLint success;
	glGetProgramiv(a_Program, a_ErrorType, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(a_Program, 512, NULL, infoLog);
		printf("ERROR::SHADER::_%i_::%s\n%s\n", a_Program, a_ErrorMessage, infoLog);
	}
	return !success;
}

bool Shader::checkGlErrorShader(const int a_ErrorType, const unsigned int a_Shader, const char * a_ErrorMessage) {
	GLint success;
	glGetShaderiv(a_Shader, a_ErrorType, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetShaderInfoLog(a_Shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::_%i_::%s\n%s\n", a_Shader, a_ErrorMessage, infoLog);
	}
	return !success;
}
