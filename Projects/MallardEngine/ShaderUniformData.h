#pragma once
#include "DLLBuild.h"
//have tested varidic functions, cant seem to get floats and ints to works the same

	//todo after the data is edited turn that uniform dirty
	//then when rendering something or using the shader update all variables that are dirty

#include <string>

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
struct DLL_BUILD ShaderUniformData {
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
public:
	//copy's the data from a_NewData into the memory at m_Data
	//unknown results if you put in data that 
	void setData(const void* a_NewData) {
		//copy from a_NewData to m_Data
		//total memory size of m_DataSize
		memcpy(m_Data, a_NewData, m_DataSize * m_DataCount);
	}

	//sets the data for this uniform using 4 floats
	//does no checks to see if there is enough memory allocated
	//or if the data is a float
	void setData(const float a_V0, const float a_V1, const float a_V2, const float a_V3) {
		//cast to float ptr
		//and index using the float to go through the data
		((float*) m_Data)[0] = a_V0;
		((float*) m_Data)[1] = a_V1;
		((float*) m_Data)[2] = a_V2;
		((float*) m_Data)[3] = a_V3;
	}

	//copys the float from a_Data
	//into m_Data with the array index of a_Offset
	//does no checks to see if there is enough memory allocated
	//or if the data is a float
	void modifyData(int a_Offset, const float a_Data) {
		((float*) m_Data)[a_Offset] = a_Data;
	}

	//copys the data from a_Data into this uniform
	//allows for offset for where the data will be placed in the uniform (a_Offset)
	//the size/amount of data you want to copy to the uniform (a_Size)
	//a offset for the copy data (a_Data and a_DataOffset)
	//vec4 example: uniform data starts at 1, 1, 1, 1
	//float rgb[] = {0.9f,0.5f,0.4f};
	//uniform->modifyData(0, &rgb, 3, 0);
	//now the uniform is 0.9, 0.5, 0.4, 1
	//note, it does not check if the data is out of range for both offsets and for a_Size
	void modifyData(const unsigned int a_Offset, const void* a_Data, const unsigned int a_Size, const unsigned int a_DataOffset = 0) {
		//so whats going on here?
		//converting m_Data from void* to char*
		//because the size of a char is 1
		//then using [] to get to the start of the data we want to change
		//since it's a char, and increments by 1 we have to multiply by the data size
		//then we use & to get the reference of the data 
		void* to = &((char*) m_Data)[a_Offset * m_DataSize];//our data
		void* from = &((char*) a_Data)[a_DataOffset * m_DataSize];//passed in data

		memcpy(to, from, a_Size * m_DataSize);//and finally the copy with size of dataSize * data

		//Alternate version
		//does that same thing, but limits it to float only

		//for (int i = 0; i < a_Size; i++) {
		//	((float*) m_Data)[a_Offset + i] = ((float*) a_Data)[a_DataOffset + i];
		//}
	}
};

