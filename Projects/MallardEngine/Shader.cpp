#include "Shader.h"

#include <GL\glew.h>
#include <stdio.h>

#include <fstream> //std::ifstream
#include <sstream> //std::stringstream
#include <string> //std::string

//for auto assigning uniforms for shaders
#include "TimeHandler.h"
#include "Window.h"
#include "Framebuffer.h"

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

	createShader(a_Type, shaderText);
}

void Shader::setFromText(ShaderTypes a_Type, const char * a_ShaderText) {
	//reset file path
	m_Shaders[(int) a_Type].m_FilePath = "";
	m_Shaders[(int) a_Type].m_LoadedFromText = a_ShaderText;


	createShader(a_Type, m_Shaders[(int) a_Type].m_LoadedFromText);
}

//some very basic shaders for a easy way to display and modify the environment
//two options for the fragment shader, one that allows a diffuse texture
//and another that just has a solid color
void Shader::createSimpleShader(bool a_Textured) {
	std::string vertex = R"(
		#version 410												
		layout(location = 0) in vec4 position;						
		layout(location = 2) in vec2 texCoord;						
		layout(location = 3) in vec4 vertColor;						
																	
		uniform mat4 projectionViewMatrix;							
		uniform mat4 model = mat4(1);								
																	
		out vec2 vTexCoord;											
		out vec4 vVertColor;											
																	
		void main() {												
			gl_Position = projectionViewMatrix * model * position;	
			vTexCoord = texCoord; 									
			vVertColor = vertColor; 									
		}
	)";

	std::string fragment;

	if (a_Textured) {
		fragment = R"(
			#version 410
			 
			in vec2 vTexCoord; 
			in vec4 vVertColor;		
									
			out vec4 fragColor; 
			 
			uniform vec4 color = vec4(1,1,1,1); 
			uniform sampler2D TexDiffuse1; 
			 
			void main() { 
				vec4 col = texture(TexDiffuse1, vTexCoord); 
			//	fragColor = (col * color)*col.a; 
			//	fragColor = vec4(col.xyz,1); 
				fragColor = col * color * vVertColor; 
			} 
		)";
	} else {
		fragment = R"(
			#version 410
			 
			in vec4 vVertColor;											

			uniform vec4 color = vec4(1,1,1,1); 
			out vec4 fragColor; 
			 
			void main() { 
				fragColor = color * vVertColor; 
			} 
		)";
	}

	setFromText(ShaderTypes::TYPE_VERTEX, vertex.c_str());
	setFromText(ShaderTypes::TYPE_FRAGMENT, fragment.c_str());
	linkShader();

}

void Shader::reloadShaders() {
	//_ASSERT_EXPR(m_Linked, "Cant reload shader, when it hasent been linked");
	m_Linked = false;

	//reset m_CommonUniforms
	m_CommonUniforms = CommonUniforms();

	for (unsigned int i = 0; i < SHADER_TYPES_SIZE; i++) {
		ShaderData* sd = &m_Shaders[i];
		if (sd->m_ShaderID != 0) {
			glDeleteShader(sd->m_ShaderID);
			sd->m_ShaderID = 0;
		}
		//if loaded from text
		if (sd->m_FilePath == "") {
			setFromText((ShaderTypes) i, sd->m_LoadedFromText.c_str());
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
	printf("Linking Shader: %s, %s\n", m_Shaders[(int) ShaderTypes::TYPE_VERTEX].m_FilePath.c_str(), m_Shaders[(int) ShaderTypes::TYPE_FRAGMENT].m_FilePath.c_str());
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
		printf("Shader %u has failed to link!\n", m_Program);
		return;
	}
	m_Linked = true;

	//load all uniforms attached to the program
	getShaderUniforms();

	printf("Shader %u has been successfully linked!\n", m_Program);
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

	//assign the resolution 
	//(could make this a common uniform between all shaders and only change when window it resized)
	if (a_Shader->m_CommonUniforms.m_Resolution != nullptr) {
		Framebuffer* fb = Framebuffer::getCurrentFramebuffer();
		if (fb != nullptr) {
			float resulution[2] = { (float) fb->getFramebufferWidth(), (float) fb->getFramebufferHeight() };
			a_Shader->m_CommonUniforms.m_Resolution->setData(resulution);
		} else {
			Window* mainWin = Window::getMainWindow();
			float resulution[2] = { (float) mainWin->getFramebufferWidth(), (float) mainWin->getFramebufferHeight() };
			a_Shader->m_CommonUniforms.m_Resolution->setData(resulution);
		}
		a_Shader->applyUniform(a_Shader->m_CommonUniforms.m_Resolution);
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

#ifdef _DEBUG
	//checking to see if this Uniform belongs to the current shader
	if (a_Data->m_ParentShader != m_LastUsed) {
		printf("ERROR WITH SHADER, SETTING UNIFORM DATA %i, IT IS NOT PART OF THE CURRENT SHADER. Name: %s\n",
			   a_Data->m_Type, a_Data->m_Name.c_str());
		return;
	}
#endif // DEBUG


	//check to see if the data has been modified
	if (a_Data->m_IsDirty) {
		a_Data->m_IsDirty = false;
		//gets uniform location
		int loc = a_Data->m_UniformLocation;
		//how many elements in the array is this
		int amount = a_Data->m_ArraySize;
		//switch case to find which glUniform... call to make
		switch (a_Data->m_Type) {
			case ShaderUniformTypes::MAT4:
				glUniformMatrix4fv(loc, amount, GL_FALSE, (float*) a_Data->m_Data);
				break;
			case ShaderUniformTypes::VEC4:
				glUniform4fv(loc, amount, (float*) a_Data->m_Data);
				break;
			case ShaderUniformTypes::VEC3:
				glUniform3fv(loc, amount, (float*) a_Data->m_Data);
				break;
			case ShaderUniformTypes::VEC2:
				glUniform2fv(loc, amount, (float*) a_Data->m_Data);
				break;
			case ShaderUniformTypes::FLOAT:
				glUniform1fv(loc, amount, (float*) a_Data->m_Data);
				break;
			case ShaderUniformTypes::SAMPLER2D:
				glUniform1iv(loc, amount, (int*) a_Data->m_Data);
				break;
			default:
				printf("ERROR WITH SHADER, SETTING UNIFORM DATA %i, Name: %s\n", a_Data->m_Type, a_Data->m_Name.c_str());
				break;
		}
	}
}

void Shader::checkUniformChanges() {
	if (m_LastUsed == nullptr) {
		return;
	}
	//go through shader types
	for (int i = 0; i < SHADER_UNIFORMS_TYPES_SIZE; i++) {
		//go through vector for that type
		for (size_t q = 0; q < m_LastUsed->m_UniformData[i].size(); q++) {
			//get shader uniform
			ShaderUniformData* uData = m_LastUsed->m_UniformData[i][q];
			//apply uniform does the dirty check
			applyUniform(uData);
		}
	}
}

void Shader::setPreprocessor(std::string a_Name) {
	m_Preprocessors[a_Name] = 0;
}

void Shader::setPreprocessor(std::string a_Name, float a_Value) {
	m_Preprocessors[a_Name] = a_Value;
}

void Shader::removePreprocessor(std::string a_Name) {
	m_Preprocessors.erase(a_Name);
}

float Shader::getPreprocessorValue(std::string a_Name) {
	if (m_Preprocessors.find(a_Name) != m_Preprocessors.end()) {
		return m_Preprocessors[a_Name];
	}
	return -1;
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
void Shader::createShader(ShaderTypes a_Type, std::string a_Code) {
	//only run this if there is a preprocessor added to this shader
	if (m_Preprocessors.size() != 0) {
		//find the new line character after #version
		size_t versionPoint = a_Code.find_first_of("#version") + 8;//8 is the length of #version
		for (int i = versionPoint; i < a_Code.size(); i++) {
			if (a_Code.at(i) == '\n') {
				versionPoint = i + 1;
				break;
			}
		}
		//now add all the preprocessors into it's own string
		//could calulate this part before hand??
		std::string preprocessor;
		std::map <std::string, float>::iterator it = m_Preprocessors.begin();
		if (it != m_Preprocessors.end()) {
			preprocessor += "#define " + it->first + " " + std::to_string(it->second) + "\n";
		}
		//finaly add the preprocessor string to the shader program code after the #version
		a_Code.insert(versionPoint, preprocessor);
	}

	//glShaderSource takes a const char* cosnt*
	//this is how
	const char* shaderCode = a_Code.c_str();
	const char* const * shaderCodePtr = &shaderCode;

	unsigned int shaderIndex = glCreateShader(getOpenglShaderType(a_Type));
	glShaderSource(shaderIndex, 1, shaderCodePtr, 0);
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
		printf("ERROR::SHADER::_%u_::%s\n%s\n", a_Program, a_ErrorMessage, infoLog);
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
		printf("ERROR::SHADER::_%u_::%s\n%s\n", a_Shader, a_ErrorMessage, infoLog);
	}
	return !success;
}

//sets all the programs uniform data into the m_UniformData
//gets information from OpenGL about the uniforms
//it also creates the memory for each data type
void Shader::getShaderUniforms() {
	//reset m_CommonUniforms
	m_CommonUniforms = CommonUniforms();

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
			case GL_FLOAT_VEC2:
				uniformData->m_DataSize = sizeof(float);
				uniformData->m_DataCount = 2;
				uniformData->m_Data = new float[2]{ 0 };
				uniformData->m_Type = ShaderUniformTypes::VEC2;
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
		uniformData->m_Name = name;
		uniformData->m_ArraySize = size;

		//add the uniform data to the full list
		m_UniformData[(int) uniformData->m_Type].push_back(uniformData);

		//setting up common uniforms
		//todo move to array for easy setting/comparisons
		//there is a better way of doing what is below...

		//compare uniform name against name of the common uniforms
		//if it matches then it will set the common uniform to reference the uniform data
		for (unsigned int i = 0; i < CommonUniforms::NUM_OF_COMMON_UNIFORMS; i++) {
			if ((&m_CommonUniforms.m_ProjectionViewMatrix)[i] == nullptr) {
				if (strcmp(name, m_CommonUniforms.m_UniformNames[i]) == 0) {
					(&m_CommonUniforms.m_ProjectionViewMatrix)[i] = uniformData;
					break;
				}
			}
		}

		//copy defaults from the shader
		switch (uniformData->m_Type) {
			case ShaderUniformTypes::MAT4:
			case ShaderUniformTypes::VEC4:
			case ShaderUniformTypes::VEC3:
			case ShaderUniformTypes::VEC2:
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
