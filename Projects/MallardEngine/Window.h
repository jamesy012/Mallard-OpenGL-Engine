#pragma once

#include "DLLBuild.h"

struct GLFWwindow;

class DLL_BUILD Window {
public:
	Window();
	~Window();
	//will use that window as a base for this class
	Window(GLFWwindow* a_Window);

	void setTitle(const char* a_Title);

	//creates a hidden window that will share the glfw context of a_ShareContext
	bool createWindow(Window* a_ShareContext);

	bool createWindow(int a_Width, int a_Height);
	bool createWindow(int a_Width, int a_Height, const char* a_Title);

	GLFWwindow* getWindow() const;
	GLFWwindow* getMainWindow() const;

	void setCallbacks() const;

private:
	/* CALLBACKS */
	static void windowSizeCallback(GLFWwindow* a_Window, int a_Width, int a_Height);
	static void windowFramebufferSizeCallback(GLFWwindow* a_Window, int a_Width, int a_Height);
	static void windowFocusCallback(GLFWwindow* a_Window, int a_Focused);

	static Window* getWindowFromGlfwWindow(GLFWwindow* a_Window);

	void checkIfWindowShouldBeMain();

	GLFWwindow* m_ThisWindow = nullptr;
	const char* m_Title = "Default Title";
};

