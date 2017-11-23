#include "ShaderUniformData.h"

//for glm::value_ptr
#include <glm\ext.hpp>
#include <glm\glm.hpp>

#include "Transform.h"
#include "Camera.h"

//copys data with a simple memcpy using the data size * how much data
//modify's data so it will make the uniform dirty(different from the value on the gpu)
void ShaderUniformData::setData(const void * a_NewData) {
	//copy from a_NewData to m_Data
	//total memory size of m_DataSize
	memcpy(m_Data, a_NewData, m_DataSize * m_DataCount);

	setDirty();
}

//simple copy for passing in 4 floats
//modify's data so it will make the uniform dirty(different from the value on the gpu)
void ShaderUniformData::setData(const float a_V0, const float a_V1, const float a_V2, const float a_V3) {
	//cast to float ptr
	//and index using the float to go through the data
	((float*) m_Data)[0] = a_V0;
	((float*) m_Data)[1] = a_V1;
	((float*) m_Data)[2] = a_V2;
	((float*) m_Data)[3] = a_V3;

	setDirty();
}

//applys a_Data into the array index of a_Offset
//only works for floats
//(could use the bigger modifyData function, although it would be a little slower)
//modify's data so it will make the uniform dirty(different from the value on the gpu)
void ShaderUniformData::modifyData(int a_Offset, const float a_Data) {
	((float*) m_Data)[a_Offset] = a_Data;

	setDirty();
}

//copys one array into another array
//allows for offsetting of both arrays
//and also for the size/amount to copy from a_Data
//works for all formats, although if a_Data is a different type then the uniform
//could easily result in a out of bounds or screw up the data
//modify's data so it will make the uniform dirty(different from the value on the gpu)
void ShaderUniformData::modifyData(const unsigned int a_Offset, const void * a_Data, const unsigned int a_Size, const unsigned int a_DataOffset) {
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

	setDirty();
}

//applys data from the mat4 into this
void ShaderUniformData::setData(const glm::mat4 * a_Mat4) {
	if (m_Type == ShaderUniformTypes::MAT4) {
		//uses value_ptr to get the start of the data
		setData(glm::value_ptr(*a_Mat4));
	}
}

void ShaderUniformData::setData(Transform * a_Data) {
	if (m_Type == ShaderUniformTypes::MAT4) {
		//gets global matrix mat4 then uses value_ptr to get the start of the data
		setData(glm::value_ptr(a_Data->getGlobalMatrix()));
	}
}

void ShaderUniformData::setData(Camera * a_Data) {
	if (m_Type == ShaderUniformTypes::MAT4) {
		//gets global matrix mat4 then uses value_ptr to get the start of the data
		setData(glm::value_ptr(a_Data->getProjectionViewMatrix()));
	}
}

void * ShaderUniformData::getDataVoid() {
	return m_Data;
}

