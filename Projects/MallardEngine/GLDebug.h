#pragma once

#include <GL\glew.h>



//https://www.khronos.org/opengl/wiki/Debug_Output

#ifdef _DEBUG
#define GLDebug_NAMEOBJ(type, id, name) GLDebug::nameGLObject(type,id,name) 
#else
#define GLDebug_NAMEOBJ(type, id, name)
#endif // _DEBUG

class GLDebug {
public:
	static void __stdcall openGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	static void nameGLObject(GLenum a_Type, GLuint a_Id, const char* a_Name);
};

