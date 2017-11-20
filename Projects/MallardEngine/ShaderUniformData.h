#pragma once
#include "DLLBuild.h"

//have tested varidic functions, cant seem to get floats and ints to works the same
//#include <stdarg.h> 

	//todo after the data is edited turn that uniform dirty
	//then when rendering something or using the shader update all variables that are dirty

#include <string>

//glm forward declare
#include <glm/fwd.hpp>

class Transform;


//the different kind of types a shader can be
//at the moment, this is missing a lot of types
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

//forward declare for friend Shader
class Shader;

//used to store and modify uniform information
class DLL_BUILD ShaderUniformData {
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
	//size of the type of memory in m_Data
	//vec4 example: sizeof(float) (will equal 4)
	unsigned int m_DataSize;
	//amount of data we have
	//vec4 example: 4
	unsigned int m_DataCount;
	//contains how many elements in an array this item has
	//will be 1 if it's not in an array
	unsigned int m_ArraySize;
	//a flag to check if this uniform is dirty
	//when dirty, it means this data has been modified and has not been used since
	bool m_IsDirty = false;
	//where did this uniform come from?
	Shader* m_ParentShader = nullptr;
public:
	//copy's the data from a_NewData into the memory at m_Data
	//unknown results if you put in data that is not of the same type
	//or more or less then required
	void setData(const void* a_NewData);

	//sets the data for this uniform using 4 floats
	//does no checks to see if there is enough memory allocated
	//or if the data is a float
	void setData(const float a_V0, const float a_V1, const float a_V2, const float a_V3);

	//copys the float from a_Data
	//into m_Data with the array index of a_Offset
	//does no checks to see if there is enough memory allocated
	//or if the data is a float
	void modifyData(int a_Offset, const float a_Data);

	//copys the data from a_Data into this uniform
	//allows for offset for where the data will be placed in the uniform (a_Offset)
	//the size/amount of data you want to copy to the uniform (a_Size)
	//a offset for the copy data (a_Data and a_DataOffset)
	//vec4 example: uniform data starts at 1, 1, 1, 1
	//float rgb[] = {0.9f,0.5f,0.4f};
	//uniform->modifyData(0, &rgb, 3, 0);
	//now the uniform is 0.9, 0.5, 0.4, 1
	//note, it does not check if the data is out of range for both offsets and for a_Size
	void modifyData(const unsigned int a_Offset, const void* a_Data, const unsigned int a_Size, const unsigned int a_DataOffset = 0);

	///Setting Data with types

	//allowing easy setting for the mat4 data type
	//does checks to see if the uniform is a mat4, it it's not then this wont do anything
	void setData(const glm::mat4* a_Data);

	//allowing easy setting for the Transform data type
	//it uses the transforms getGlobalMatrix
	//does checks to see if the uniform is a mat4, it it's not then this wont do anything
	void setData(Transform* a_Data);

	///Getting Data

	//returns a pointer to the data
	void* getDataVoid();
private:
	//sets this uniform as dirty
	//use this after data has been modified
	void setDirty() {
		m_IsDirty = true;
	}
	
};

