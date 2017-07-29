#pragma once
#include "DLLBuild.h"

//for the ShaderUniformData variadic set data function
#include <stdarg.h>

#include <string>
#include <vector>

enum class ShaderTypes {
	TYPE_VERTEX,
	TYPE_FRAGMENT,
	END_TYPES
};
static const int SHADER_TYPES_SIZE = (int) ShaderTypes::END_TYPES;

enum class ShaderUniformTypes {
	MAT4,
	VEC3,
	VEC4,
	FLOAT,
	INT,
	BOOL,
	SAMPLER2D,
	END_UNIFORM_TYPES
};
static const int SHADER_UNIFORMS_TYPES_SIZE = (int) ShaderUniformTypes::END_UNIFORM_TYPES;

//forward deceleration
struct ShaderUniformData;

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
	//also sets up the uniforms for which ones can be used or not
	void linkShader();

	//gets the program id for this shader
	//will be nullptr if unused
	unsigned int getProgram();

	//will use shader program
	void use();

	//returns the last used shader,
	//will be nullptr if no shader has been used yet
	static Shader* getCurrentShader();

	ShaderUniformData* getUniform(ShaderUniformTypes a_Type, const char* a_Name);

	void applyUniform(ShaderUniformData* a_Data);

private:
	//Converts a ShaderTypes into the opengl version of that Shader
	//input: ShaderTypes::TYPE_VERTEX 
	//output: GL_VERTEX_SHADER define from opengl header
	unsigned int getOpenglShaderType(ShaderTypes a_Type);
	//Creates and compiles a shader using the type and code provided
	//will print a error if there was a issue with the shader code provided
	void createShader(ShaderTypes a_Type, const char* const* a_Code);
	//Gets the error from opengl and prints it
	//errorMessage appears before the error log so you can work out what function is having the issue
	//example ERROR::SHADER::ProgramID \n a_ErrorMessage \n {error log}
	static bool checkGlErrorProgram(const int a_ErrorType, const unsigned int a_Program, const char* a_ErrorMessage = "");
	//Gets the error from opengl and prints it
	//errorMessage appears before the error log so you can work out what function is having the issue
	//example ERROR::SHADER::ShaderID \n a_ErrorMessage \n {error log}
	static bool checkGlErrorShader(const int a_ErrorType, const unsigned int a_Shader, const char* a_ErrorMessage = "");

	//Finds all uniforms attached to this shader
	void getShaderUniforms();

	//id of the current program
	//set up after linkShader or create program
	unsigned int m_Program;
	//list of all shaders for this program
	//after linking these are reset to 0 as the shaders are deleted
	//used to store a reference for each shader type for the shader
	//use ShaderTypes to reference certain shaders
	unsigned int m_Shaders[SHADER_TYPES_SIZE];
	//is this program linked to it's shaders
	//and can be used?
	bool m_Linked;

	//shader data split up by it's shader type
	std::vector<ShaderUniformData*> m_UniformData[SHADER_UNIFORMS_TYPES_SIZE];
};

//used to store and modify uniform information
struct DLL_BUILD ShaderUniformData {
	//todo after the data is edited turn that uniform dirty
	//then when rendering something or using the shader update all variables that are dirty
	friend Shader;
private:
	//pointer for the data
	//void* to allow different types of data
	void* m_Data;
	//type of this data
	//used in conjunction with m_Data so you can tell what type it is
	ShaderUniformTypes m_Type;
	//name of the uniform in the shader
	std::string m_Name;
	//location of the uniform on the shader
	unsigned int m_UniformLocation;
	//size of the memory in m_Data
	//example for vec4: 4 * sizeof(float)
	unsigned int m_DataSize;
public:
	//copy's the data from a_NewData into the memory at m_Data
	//unknown results if you put in data that 
	void setData(void* a_NewData) {
		//copy from a_NewData to m_Data
		//total memory size of m_DataSize
		memcpy(m_Data, a_NewData, m_DataSize);
	}
	//Varidaic version for setting data
	//to allow the ability to set values separately 
	void setDataVaridaic(int a_Count, ...) {
		va_list ap;
		va_start(ap, a_Count);
		for (int i = 0; i < a_Count; i++) {
			void* value = va_arg(ap, void*);
			m_Data = &value;
		}
		va_end(ap);
	}
};
