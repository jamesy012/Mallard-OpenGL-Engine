#pragma once
#include "DLLBuild.h"

#include <vector>

#include "ShaderUniformData.h"

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

	//finds a uniform with the name of a_Name, and the type of a_Type
	//returns nullptr if no uniform found
	ShaderUniformData* getUniform(ShaderUniformTypes a_Type, const char* a_Name);

	//apply a_Data to the shader that is currently in use
	//it uses the type of a_Data to find out which opengl call to use
	//will cause a error if a_Data is not a ShaderUniformData or if its nullptr
	static void applyUniform(ShaderUniformData* a_Data);

	//list of common uniforms for easy and quick access to them
	//these are set up after linking
	struct {
		//direct reference to the projectionView Matrix
		//this is a mat4, in the Vertex Shader
		//it is used to place the camera around the 
		ShaderUniformData* m_ProjectionViewMatrix = nullptr;
		//direct reference to the Model matrix
		//this is a mat4, in the Vertex Shader
		//it is used to set the position/rotation and scale of the current model
		ShaderUniformData* m_ModelMatrix = nullptr;
		//direct reference to the rgba color of the object
		//this is a vec4, in the Fragment Shader
		//it is used to modify the color of the object
		ShaderUniformData* m_Color = nullptr;
	} m_CommonUniforms;

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
