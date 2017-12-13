#pragma once
#include "DLLBuild.h"

#include <vector>
#include <string>

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

	///PUBLIC FUNCTIONS

	///LOADING SHADERS

	//sets up the shader a_Type using the text from the file at a_FilePath then compiles the shader
	void setFromPath(ShaderTypes a_Type, const char* a_FilePath);
	//sets up the shader a_Type using the text from a_ShaderText then compiles the shader
	void setFromText(ShaderTypes a_Type, const char* a_ShaderText);

	//sets up a simple shader
	//with a bool to make it textured or not
	void createSimpleShader(bool a_Textured);

	void reloadShaders();

	///SETTING UP SHADERS FOR USE

	//creates program for this shader
	void createProgram();

	//links the shaderPrograms together
	//will only link shaders that have been set
	//will create program if there is none
	//also sets up the uniforms for which ones can be used or not
	void linkShader();

	///USING SHADERS

	//gets the program id for this shader
	//will be nullptr if unused
	unsigned int getProgram() const;

	//will use shader program
	static void use(const Shader* a_Shader);

	//returns the last used shader,
	//will be nullptr if no shader has been used yet
	static const Shader* getCurrentShader();

	///UNIFORM FUNCTIONS

	//finds a uniform with the name of a_Name, and the type of a_Type
	//returns nullptr if no uniform found
	ShaderUniformData* getUniform(const ShaderUniformTypes a_Type, const char* a_Name) const;

	//apply a_Data to the shader that is currently in use
	//it uses the type of a_Data to find out which opengl call to use
	//might cause a error if a_Data is not a ShaderUniformData (untested)
	//if a_Data is nullptr, this function wont do anything
	static void applyUniform(ShaderUniformData* a_Data);

	//checks all the current shaders uniform to see if any have changed
	//and applys the if they have
	static void checkUniformChanges();

	///PUBLIC DATA

	//list of common uniforms for easy and quick access to them
	//these are set up after linking
	struct CommonUniforms {
		friend Shader;
	protected:
		static const unsigned int NUM_OF_COMMON_UNIFORMS = 7u;
		char* m_UniformNames[NUM_OF_COMMON_UNIFORMS]{
			"projectionViewMatrix",
			"model",
			"normalRot",
			"color",
			"time",
			"resolution",
			"TexDiffuse1",
		};
		ShaderUniformData** m_FirstUSD;
	public:
		//direct reference to the projectionView Matrix
		//this is a mat4, in the Vertex Shader
		//it is used to place the camera around the 
		ShaderUniformData* m_ProjectionViewMatrix = nullptr;
		//direct reference to the Model matrix
		//this is a mat4, in the Vertex Shader
		//it is used to set the position/rotation and scale of the current model
		ShaderUniformData* m_ModelMatrix = nullptr;
		//direct reference to the Model matrix
		//this is a mat4, in the Vertex Shader
		//it is used to set the position/rotation and scale of the current model
		ShaderUniformData* m_NormalRotMatrix = nullptr;
		//direct reference to the rgba color of the object
		//this is a vec4, in the Fragment Shader
		//it is used to modify the color of the object
		ShaderUniformData* m_Color = nullptr;
		//Time of the program since the start
		ShaderUniformData* m_Time = nullptr;
		//Resulution of the main window
		ShaderUniformData* m_Resolution = nullptr;
		ShaderUniformData* m_Textures[1]{ nullptr };
	} m_CommonUniforms;

private:
	///PRIVATE FUNCTIONS

	//Converts a ShaderTypes into the opengl version of that Shader
	//input: ShaderTypes::TYPE_VERTEX 
	//output: GL_VERTEX_SHADER define from opengl header
	unsigned int getOpenglShaderType(ShaderTypes a_Type) const;

	//Creates and compiles a shader using the type and code provided
	//will print a error if there was a issue with the shader code provided
	void createShader(ShaderTypes a_Type, const char* const* a_Code);

	///ERRORS

	//Gets the error from opengl and prints it
	//errorMessage appears before the error log so you can work out what function is having the issue
	//example ERROR::SHADER::ProgramID \n a_ErrorMessage \n {error log}
	static bool checkGlErrorProgram(const int a_ErrorType, const unsigned int a_Program, const char* a_ErrorMessage = "");

	//Gets the error from opengl and prints it
	//errorMessage appears before the error log so you can work out what function is having the issue
	//example ERROR::SHADER::ShaderID \n a_ErrorMessage \n {error log}
	static bool checkGlErrorShader(const int a_ErrorType, const unsigned int a_Shader, const char* a_ErrorMessage = "");

	///UNIFORMS

	//Finds all uniforms attached to this shader
	//sets up m_UniformData
	//sets up m_CommonUniforms
	//also gets the default values from the shader
	void getShaderUniforms();

	///PRIVATE DATA

	//id of the current program
	//set up after linkShader or create program
	unsigned int m_Program;

	//list of all shaders for this program
	//use ShaderTypes to reference certain shaders
	struct ShaderData {
		//id for the shader
		//after linking these are reset to 0 as the shaders are deleted
		//used to store a reference for each shader type for the shader
		unsigned int m_ShaderID = 0;
		//What was the type of this shader
		ShaderTypes m_ShaderType;
		//path to the file
		//if the shader was not loaded from a file path then it will be empty
		//if the shader fails to load/find the file, this will still have the path
		std::string m_FilePath = "";
		//copy of the shader that was loaded from text
		std::string m_LoadedFromText = "";
	} m_Shaders[SHADER_TYPES_SIZE];

	//is this program linked to it's shaders
	//and can be used?
	bool m_Linked;

	//shader data split up by it's shader type
	std::vector<ShaderUniformData*> m_UniformData[SHADER_UNIFORMS_TYPES_SIZE];
};
