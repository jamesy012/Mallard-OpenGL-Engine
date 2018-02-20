#include "GLDebug.h"

#include <string>
#include <stdio.h>

void GLDebug::openGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam) {
	std::string errorMessage;
	switch (source) {
		case GL_DEBUG_SOURCE_API:
			errorMessage += "DEBUG_SOURCE_API/";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			errorMessage += "DEBUG_SOURCE_WINDOW_SYSTEM/";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			errorMessage += "DEBUG_SOURCE_SHADER_COMPILER/";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			errorMessage += "DEBUG_SOURCE_THIRD_PARTY/";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			errorMessage += "DEBUG_SOURCE_APPLICATION/";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			errorMessage += "DEBUG_SOURCE_OTHER/";
			break;
	}
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			errorMessage += "DEBUG_TYPE_ERROR/";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			errorMessage += "DEBUG_TYPE_DEPRECATED_BEHAVIOR/";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			errorMessage += "DEBUG_TYPE_UNDEFINED_BEHAVIOR/";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			errorMessage += "DEBUG_TYPE_PORTABILITY/";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			errorMessage += "DEBUG_TYPE_PERFORMANCE/";
			break;
		case GL_DEBUG_TYPE_MARKER:
			errorMessage += "DEBUG_TYPE_MARKER/";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			errorMessage += "DEBUG_TYPE_PUSH_GROUP/";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			errorMessage += "DEBUG_TYPE_POP_GROUP/";
			break;
		case GL_DEBUG_TYPE_OTHER:
			errorMessage += "DEBUG_TYPE_OTHER/";
			break;
	}
	switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			errorMessage += "DEBUG_SEVERITY_NOTIFICATION/";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			errorMessage += "DEBUG_SEVERITY_MEDIUM/";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			errorMessage += "DEBUG_SEVERITY_LOW/";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			errorMessage += "DEBUG_SEVERITY_HIGH/";
			break;
	}
	printf("GL_ERROR: %X(%i), %s\n\t%s\n", id,id, errorMessage.c_str(), message);
	//printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
	//	(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
	//	   type, severity, message);
}

void GLDebug::nameGLObject(GLenum a_Type, GLuint a_Id, const char* a_Name) {
	glObjectLabel(a_Type, a_Id, -1, a_Name);
}
