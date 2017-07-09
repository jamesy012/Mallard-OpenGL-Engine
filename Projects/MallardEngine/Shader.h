#pragma once
#include "DLLBuild.h"

enum ShaderTypes {
	TYPE_VERTEX,
	TYPE_FRAGMENT,
	END_TYPES
};

class DLL_BUILD Shader {
public:
	Shader();
	~Shader();

	void setFromPath(ShaderTypes a_Type, const char* a_FilePath);
	void setFromText(ShaderTypes a_Type, const char* a_ShaderText);

	void createSimpleShader(bool a_Textured);

	void createProgram();
	void linkShader();

	unsigned int getProgram();

	void use();

	static Shader* getCurrentShader();

private:
	unsigned int getOpenglShaderType(ShaderTypes a_Type);
	void createShader(ShaderTypes a_Type, const char* const* a_Code);

	unsigned int m_Program;
	unsigned int m_Shaders[ShaderTypes::END_TYPES];
	bool m_Linked;
};

