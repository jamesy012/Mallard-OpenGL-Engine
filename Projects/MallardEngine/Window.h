#pragma once

#include "DLLBuild.h"

struct GLFWwindow;
class Input;

class DLL_BUILD Window {
	friend Input;
public:
	Window();
	~Window();
	//will use that window as a base for this class
	Window(GLFWwindow* a_Window);

	//temporary function pointer for window sizes
	typedef void (*WindowResizeFn) (int a_Width, int a_Height);
	WindowResizeFn m_WindowResizeCallback;
	WindowResizeFn m_WindowResizeFramebufferCallback;

	void destroyWindow();

	/*** Window Creation Functions */
	//creates a hidden window that will share the glfw context of a_ShareContext
	bool createWindow(Window* a_ShareContext);

	//creates a hidden window with a new glfw context
	bool createWindow();

	bool createWindow(int a_Width, int a_Height);
	bool createWindow(int a_Width, int a_Height, const char* a_Title, bool a_Visable = true);

	void setVisable(bool a_IsVisable);

	void setWindowSize(int a_width, int a_Height);
	void setWindowWidth(int a_width);
	void setWindowHeight(int a_Height);
	void setWindowTitle(const char* a_Title);
	void setWindowData(int a_width, int a_Height,const char* a_Title);

	GLFWwindow* getWindow() const;
	static GLFWwindow* getMainWindowGLFW();
	static Window* getMainWindow();

	void setCallbacks() const;

	/*** Window Information Functions */
	//sets title of window
	void setTitle(const char* a_Title);

	int const getWindowWidth() const;
	int const getWindowHeight() const;
	int const getFramebufferWidth() const;
	int const getFramebufferHeight() const;
	bool const isWindowCreated() const;

	bool const getIsVisable() const;

	void makeContextCurrent() const;

protected:
	static Window* getWindowFromGlfwWindow(GLFWwindow* a_Window);
private:
	/* CALLBACKS */
	static void windowSizeCallback(GLFWwindow* a_Window, int a_Width, int a_Height);
	static void windowFramebufferSizeCallback(GLFWwindow* a_Window, int a_Width, int a_Height);
	static void windowFocusCallback(GLFWwindow* a_Window, int a_Focused);

	//called after a window is created
	//sets up all the information this class needs
	void applyGlfwWindowDataToClass();

	//checks if there is currently a main window,and if this window should become it
	void checkIfWindowShouldBeMain();

	void updateWindowSize();

	GLFWwindow* getMainContext();

	GLFWwindow* m_ThisWindow = nullptr;

	const char* m_Title = "Default Title";
	int m_Width, m_Height;
	int m_FramebufferWidth, m_FramebufferHeight;
	bool m_IsVisable = false;

	bool m_IsWindowCreated = false;
};

