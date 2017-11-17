#include "Shader.h"

#include <GL\glew.h>
#include <stdio.h>

#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

#include "TimeHandler.h"

static const Shader* m_LastUsed = nullptr;

Shader::Shader() {
	m_Program = 0;
	for (int i = 0; i < SHADER_TYPES_SIZE; i++) {
		m_Shaders[i].m_ShaderID = 0;
	}
	m_Linked = false;
}


Shader::~Shader() {
	//todo delete program
	if (m_Program != 0) {
		glDeleteProgram(m_Program);
		m_Program = 0;
	}
	//go through shader types
	for (int i = 0; i < SHADER_UNIFORMS_TYPES_SIZE; i++) {
		//go through vector for that type
		for (size_t q = 0; q < m_UniformData[i].size(); q++) {
			//get shader uniform
			ShaderUniformData* uData = m_UniformData[i][q];
			//delete void* data
			delete uData->m_Data;
			//delete uniform
			delete uData;
		}
	}
}

void Shader::setFromPath(ShaderTypes a_Type, const char * a_FilePath) {
	//copy file path into our stored version
	m_Shaders[(int) a_Type].m_FilePath = a_FilePath;
	

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
	//reset file path
	m_Shaders[(int) a_Type].m_FilePath = "";
	m_Shaders[(int) a_Type].m_LoadedFromText = a_ShaderText;


	createShader(a_Type, &a_ShaderText);
}

//some very basic shaders for a easy way to display and modify the environment
//two options for the fragment shader, one that allows a diffuse texture
//and another that just has a solid color
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
			"	fragColor = col * color; "
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

void Shader::reloadShaders() {
	//_ASSERT_EXPR(m_Linked, "Cant reload shader, when it hasent been linked");
	m_Linked = false;
	for (unsigned int i = 0; i < SHADER_TYPES_SIZE; i++) {
		ShaderData* sd = &m_Shaders[i];
		if (sd->m_ShaderID != 0) {
			glDeleteShader(sd->m_ShaderID);
			sd->m_ShaderID = 0;
		}
		//if loaded from text
		if (sd->m_FilePath == "") {
			setFromText((ShaderTypes)i, sd->m_LoadedFromText.c_str());
		} else {//else loaded from file
			setFromPath((ShaderTypes) i, sd->m_FilePath.c_str());
		}
	}

	//go through shader types
	for (int i = 0; i < SHADER_UNIFORMS_TYPES_SIZE; i++) {
		//go through vector for that type
		for (size_t q = 0; q < m_UniformData[i].size(); q++) {
			//get shader uniform
			ShaderUniformData* uData = m_UniformData[i][q];
			//delete void* data
			delete uData->m_Data;
			//delete uniform
			delete uData;
		}
		m_UniformData[i].clear();
	}

	linkShader();

	if (m_LastUsed == this) {
		Shader::use(this);
	}
}

void Shader::createProgram() {
	if (m_Program != 0) {
		printf("shader already had a program");
	}
	m_Program = glCreateProgram();
}

//combines all shaders into 1 program
//will delete all loaded shaders when complete
//and gets uniforms
void Shader::linkShader() {
	if (m_Linked) {
		printf("Shader already linked\n");
		return;
	}
	//create program, if it hasent already
	if (m_Program == 0) {
		createProgram();
	}

	//create shader
	for (int i = 0; i < SHADER_TYPES_SIZE; i++) {
		if (m_Shaders[i].m_ShaderID != 0) {
			glAttachShader(m_Program, m_Shaders[i].m_ShaderID);
		}
	}
	//finally link the shader
	glLinkProgram(m_Program);

	//check for errors during the link process
	bool linked = !checkGlErrorProgram(GL_LINK_STATUS, m_Program, "LINKING_FAILED");

	//delete the shaders, since now they are apart of the program
	for (int i = 0; i < SHADER_TYPES_SIZE; i++) {
		if (m_Shaders[i].m_ShaderID != 0) {
			glDeleteShader(m_Shaders[i].m_ShaderID);
			glDetachShader(m_Program, m_Shaders[i].m_ShaderID);
			m_Shaders[i].m_ShaderID = 0;
		}
	}
	if (!linked) {
		return;
	}
	m_Linked = true;

	//load all uniforms attached to the program
	getShaderUniforms();
}

unsigned int Shader::getProgram() const {
	return m_Program;
}

void Shader::use(const Shader* a_Shader) {
	if (m_LastUsed == a_Shader) {
		return;
	}

	//tell OpenGL to use the program
	glUseProgram(a_Shader->m_Program);
	m_LastUsed = a_Shader;

	//assign the new time
	if (a_Shader->m_CommonUniforms.m_Time != nullptr) {
		float time = TimeHandler::getCurrentTime();
		a_Shader->m_CommonUniforms.m_Time->setData(&time);
		a_Shader->applyUniform(a_Shader->m_CommonUniforms.m_Time);
	}
}

const Shader * Shader::getCurrentShader() {
	return m_LastUsed;
}

ShaderUniformData * Shader::getUniform(const ShaderUniformTypes a_Type, const char * a_Name) const {
	ShaderUniformData* uniform = nullptr;
	int uniformType = (int) a_Type;

	//go through each uniform of the given type
	//and check if it's name matches the name supplied
	for (size_t i = 0; i < m_UniformData[uniformType].size(); i++) {
		if (m_UniformData[uniformType][i]->m_Name == a_Name) {
			uniform = m_UniformData[uniformType][i];
			break;
		}
	}

	//if no uniform was found it will return nullptr
	return uniform;
}

void Shader::applyUniform(ShaderUniformData * a_Data) {
	if (a_Data == nullptr) {
		return;
	}

	//check to see if the data has been modified
	if (a_Data->m_IsDirty) {
		a_Data->m_IsDirty = false;
		//gets uniform location
		int loc = a_Data->m_UniformLocation;
		//switch case to find which glUniform... call to make
		switch (a_Data->m_Type) {
			case ShaderUniformTypes::MAT4:
				glUniformMatrix4fv(loc, 1, GL_FALSE, (float*) a_Data->m_Data);
				break;
			case ShaderUniformTypes::VEC4:
				glUniform4fv(loc, 1, (float*) a_Data->m_Data);
				break;
			case ShaderUniformTypes::FLOAT:
				glUniform1fv(loc, 1, (float*) a_Data->m_Data);
				break;
			default:
				printf("ERROR WITH SHADER, SETTING UNIFORM DATA %u, Name: %s\n", a_Data->m_Type, a_Data->m_Name.c_str());
				break;
		}
	}
}

unsigned int Shader::getOpenglShaderType(ShaderTypes a_Type) const {
	switch (a_Type) {
		case ShaderTypes::TYPE_VERTEX:
			return GL_VERTEX_SHADER;
		case ShaderTypes::TYPE_FRAGMENT:
			return GL_FRAGMENT_SHADER;
		default:
			return 0;
	}
}

//creates a shader for a_Type using the code from a_Code
void Shader::createShader(ShaderTypes a_Type, const char * const * a_Code) {
	unsigned int shaderIndex = glCreateShader(getOpenglShaderType(a_Type));
	glShaderSource(shaderIndex, 1, a_Code, NULL);
	glCompileShader(shaderIndex);

	checkGlErrorShader(GL_COMPILE_STATUS, shaderIndex, "createShader::COMPILATION_FAILED");

	m_Shaders[(int) a_Type].m_ShaderID = shaderIndex;
	m_Shaders[(int) a_Type].m_ShaderType = a_Type;
}

//Gets error message for shaders from opengl using a_ErrorType
//no checks to see if it's a real error
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

//Gets error message for shaders from opengl using a_ErrorType
//no checks to see if it's a real error
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

//sets all the programs uniform data into the m_UniformData
//gets information from OpenGL about the uniforms
//it also creates the memory for each data type
void Shader::getShaderUniforms() {
	m_CommonUniforms.m_ProjectionViewMatrix = m_CommonUniforms.m_ModelMatrix = m_CommonUniforms.m_Color = m_CommonUniforms.m_Time = nullptr;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 32; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length

	//gets number of uniforms
	glGetProgramiv(m_Program, GL_ACTIVE_UNIFORMS, &count);

	//goes through each uniform and creates a ShaderUniformData object to store and allow
	//modification of uniforms
	for (int i = 0; i < count; i++) {
		//gets uniform name and type
		glGetActiveUniform(m_Program, (GLuint) i, bufSize, &length, &size, &type, name);

		//sets up a shaderUniformData object
		ShaderUniformData* uniformData = new ShaderUniformData();
		uniformData->m_ParentShader = this;

		//gets the type and creates the data with the correct size
		switch (type) {
			case GL_FLOAT_MAT4:
				uniformData->m_DataSize = sizeof(float);
				uniformData->m_DataCount = 16;
				uniformData->m_Data = new float[16]{ 0 };
				uniformData->m_Type = ShaderUniformTypes::MAT4;
				break;
			case GL_FLOAT_VEC4:
				uniformData->m_DataSize = sizeof(float);
				uniformData->m_DataCount = 4;
				uniformData->m_Data = new float[4]{ 0 };
				uniformData->m_Type = ShaderUniformTypes::VEC4;
				break;
			case GL_FLOAT_VEC3:
				uniformData->m_DataSize = sizeof(float);
				uniformData->m_DataCount = 3;
				uniformData->m_Data = new float[3]{ 0 };
				uniformData->m_Type = ShaderUniformTypes::VEC3;
				break;
			case GL_FLOAT:
				uniformData->m_DataSize = sizeof(float);
				uniformData->m_DataCount = 1;
				uniformData->m_Data = new float[1]{ 0 };
				uniformData->m_Type = ShaderUniformTypes::FLOAT;
				break;
			case GL_SAMPLER_2D:
				uniformData->m_DataSize = sizeof(int);
				uniformData->m_DataCount = 1;
				uniformData->m_Data = new int[1]{ 0 };
				uniformData->m_Type = ShaderUniformTypes::SAMPLER2D;
				break;
			default:
				//shader type was not listed
				//log error about missing type
				delete uniformData;
				printf("ERROR WITH SHADER, GETTING UNIFORM TYPE %u, Name: %s\n", type, name);
				continue;
		}

		//gets uniform location (Appears to be the same as i)
		uniformData->m_UniformLocation = glGetUniformLocation(m_Program, name);
		uniformData-> m_Name= name;

		//add the uniform data to the full list
		m_UniformData[(int) uniformData->m_Type].push_back(uniformData);

		//setting up common uniforms
		//todo move to array for easy setting/comparisons
		//there is a better way of doing what is below...

		//compare uniform name against name of the common uniforms
		//if it matches then it will set the common uniform to reference the uniform data
		if (strcmp(name, "projectionViewMatrix") == 0) {
			m_CommonUniforms.m_ProjectionViewMatrix = uniformData;
		} else if (strcmp(name, "model") == 0) {
			m_CommonUniforms.m_ModelMatrix = uniformData;
		} else if (strcmp(name, "color") == 0) {
			m_CommonUniforms.m_Color = uniformData;
		} else if (strcmp(name, "time") == 0) {
			m_CommonUniforms.m_Time = uniformData;
		}

		//copy defaults from the shader
		switch (uniformData->m_Type) {
			case ShaderUniformTypes::MAT4:
			case ShaderUniformTypes::VEC4:
			case ShaderUniformTypes::VEC3:
			case ShaderUniformTypes::FLOAT:
				glGetUniformfv(m_Program, uniformData->m_UniformLocation, (GLfloat*) uniformData->m_Data);
				break;
			case ShaderUniformTypes::SAMPLER2D:
				glGetUniformiv(m_Program, uniformData->m_UniformLocation, (GLint*) uniformData->m_Data);
				break;
			default:
				printf("ERROR WITH SHADER, GETTING UNIFORM DEFAULT DATA %u, Name: %s\n", type, name);
				break;
		}

	}
}
