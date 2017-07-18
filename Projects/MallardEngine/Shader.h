#pragma once
#include "DLLBuild.h"

enum class ShaderTypes {
	TYPE_VERTEX,
	TYPE_FRAGMENT,
	END_TYPES
};
static const int SHADER_TYPES_SIZE = (int) ShaderTypes::END_TYPES;

class DLL_BUILD Shader {
public:
	Shader();
	~Shader();

	//sets up the shader a_Type and compiles it
	void setFromPath(ShaderTypes a_Type, const char* a_FilePath);
	//sets up the shader a_Type and compiles it
	void setFromText(ShaderTypes a_Type, const char* a_ShaderText);

	//sets up a simple shader
	//with a bool to make it textured or not
	void createSimpleShader(bool a_Textured);

	//creates program for this shader
	void createProgram();
	//links the shaderPrograms together
	//will only link shaders that have been set
	//will create program if there is none
	void linkShader();

	//gets the program id for this shader
	//will be nullptr if unused
	unsigned int getProgram();

	//will use shader program
	void use();

	//returns the last used shader,
	//will be nullptr if no shader has been used yet
	static Shader* getCurrentShader();

private:
	unsigned int getOpenglShaderType(ShaderTypes a_Type);
	void createShader(ShaderTypes a_Type, const char* const* a_Code);
	static bool checkGlErrorProgram(const int a_ErrorType, const unsigned int a_Program, const char* a_ErrorMessage = "");
	static bool checkGlErrorShader(const int a_ErrorType, const unsigned int a_Shader, const char* a_ErrorMessage = "");

	unsigned int m_Program;
	unsigned int m_Shaders[SHADER_TYPES_SIZE];
	bool m_Linked;
};

