#include "Window.h"

#include <stdio.h>

#include <map>

#include <GLFW\glfw3.h>



static Window* m_MainWindow = nullptr;

static std::map<GLFWwindow*, Window*> m_GlfwToWindowMap;

#define CHECK_IF_WINDOW() if(m_ThisWindow != nullptr){printf("Window already exists\n"); return false;} 
#define WINDOW_CREATE_RETURN() return m_ThisWindow != nullptr;
#define ADD_WINDOW_TO_MAP() m_GlfwToWindowMap[m_ThisWindow] = this;


Window::Window() {
}


Window::~Window() {
	if (this == m_MainWindow) {
		//TODO: go to next window
		m_MainWindow = nullptr;
	}
	if (m_ThisWindow != nullptr) {
		glfwDestroyWindow(m_ThisWindow);
	}
}

Window::Window(GLFWwindow * a_Window) {
	m_ThisWindow = a_Window;
	//get width/height/other window stuff
	checkIfWindowShouldBeMain();
}

void Window::setTitle(const char * a_Title) {
	glfwSetWindowTitle(m_ThisWindow, a_Title);
}

bool Window::createWindow(Window* a_ShareContext) {
	CHECK_IF_WINDOW();

	if (a_ShareContext->m_ThisWindow == nullptr) {
		return false;
	}

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	m_ThisWindow = glfwCreateWindow(640, 480, "", NULL, a_ShareContext->m_ThisWindow);
	//this wont be a main window?
	//checkIfWindowShouldBeMain();
	ADD_WINDOW_TO_MAP();
	WINDOW_CREATE_RETURN();
}

bool Window::createWindow(int a_Width, int a_Height) {
	CHECK_IF_WINDOW();
	m_ThisWindow = glfwCreateWindow(a_Width, a_Height, m_Title, NULL, NULL);
	checkIfWindowShouldBeMain();
	ADD_WINDOW_TO_MAP();
	WINDOW_CREATE_RETURN();
}

bool Window::createWindow(int a_Width, int a_Height, const char * a_Title) {
	CHECK_IF_WINDOW();
	m_ThisWindow = glfwCreateWindow(a_Width, a_Height, a_Title, NULL, NULL);
	checkIfWindowShouldBeMain();
	ADD_WINDOW_TO_MAP();
	WINDOW_CREATE_RETURN();
}

GLFWwindow * Window::getWindow() const {
	return m_ThisWindow;
}

GLFWwindow * Window::getMainWindow() const {
	return m_MainWindow->m_ThisWindow;
}

void Window::setCallbacks() const {
	glfwSetWindowSizeCallback(m_ThisWindow, Window::windowSizeCallback);
	glfwSetFramebufferSizeCallback(m_ThisWindow, Window::windowFramebufferSizeCallback);
	glfwSetWindowFocusCallback(m_ThisWindow, Window::windowFocusCallback);
}

void Window::windowSizeCallback(GLFWwindow * a_Window, int a_Width, int a_Height) {
	Window* window = getWindowFromGlfwWindow(a_Window);
}

void Window::windowFramebufferSizeCallback(GLFWwindow * a_Window, int a_Width, int a_Height) {
}

void Window::windowFocusCallback(GLFWwindow * a_Window, int a_Focused) {
}

Window * Window::getWindowFromGlfwWindow(GLFWwindow * a_Window) {
	return m_GlfwToWindowMap.at(a_Window);
}

void Window::checkIfWindowShouldBeMain() {
	if (m_ThisWindow != nullptr) {
		if (m_MainWindow == nullptr) {
			m_MainWindow = this;
		}
	}
}
