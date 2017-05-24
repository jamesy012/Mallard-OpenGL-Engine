#include "Window.h"

#include <stdio.h>
#include <map>

#include <GLFW\glfw3.h>

#include "Input.h"

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
	destroyWindow();
}

Window::Window(GLFWwindow * a_Window) {
	m_ThisWindow = a_Window;
	//get width/height/other window stuff
	checkIfWindowShouldBeMain();
}

void Window::destroyWindow() {
	if (m_IsWindowCreated) {
		glfwDestroyWindow(m_ThisWindow);
		m_FrameBufferHeight = m_FrameBufferWidth = m_Width = m_Height = 0;
		m_IsVisable = false;
	}
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
	m_ThisWindow = glfwCreateWindow(1, 1, "", NULL, a_ShareContext->m_ThisWindow);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

	//this wont be a main window
	//checkIfWindowShouldBeMain();
	applyGlfwWindowDataToClass();
	ADD_WINDOW_TO_MAP();
	WINDOW_CREATE_RETURN();
}

bool Window::createWindow() {
	CHECK_IF_WINDOW();

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	m_ThisWindow = glfwCreateWindow(1, 1, "", NULL, NULL);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

	//this wont be a main window
	checkIfWindowShouldBeMain();
	applyGlfwWindowDataToClass();
	ADD_WINDOW_TO_MAP();
	WINDOW_CREATE_RETURN();
}

bool Window::createWindow(int a_Width, int a_Height) {
	CHECK_IF_WINDOW();
	m_ThisWindow = glfwCreateWindow(a_Width, a_Height, m_Title, NULL, getMainContext());

	checkIfWindowShouldBeMain();
	applyGlfwWindowDataToClass();
	ADD_WINDOW_TO_MAP();
	WINDOW_CREATE_RETURN();
}

bool Window::createWindow(int a_Width, int a_Height, const char * a_Title) {
	CHECK_IF_WINDOW();
	m_Title = a_Title;
	m_ThisWindow = glfwCreateWindow(a_Width, a_Height, a_Title, NULL, getMainContext());

	checkIfWindowShouldBeMain();
	applyGlfwWindowDataToClass();
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
	glfwSetKeyCallback(m_ThisWindow, Input::keyCallback);
	glfwSetCursorPosCallback(m_ThisWindow, Input::cursorPositionCallback);
	glfwSetMouseButtonCallback(m_ThisWindow, Input::mouseButtonCallback);
	glfwSetCharCallback(m_ThisWindow, Input::charCallback);
	glfwSetScrollCallback(m_ThisWindow, Input::scrollCallback);
	glfwSetWindowSizeCallback(m_ThisWindow, Window::windowSizeCallback);
	glfwSetFramebufferSizeCallback(m_ThisWindow, Window::windowFramebufferSizeCallback);
	glfwSetWindowFocusCallback(m_ThisWindow, Window::windowFocusCallback);
}

int const Window::getWindowWidth() const {
	return m_Width;
}

int const Window::getWindowHeight() const {
	return m_Height;
}

bool const Window::isWindowCreated() const {
	return m_IsVisable;
}

void Window::makeContextCurrent() const {
	glfwMakeContextCurrent(m_ThisWindow);
}

void Window::windowSizeCallback(GLFWwindow * a_Window, int a_Width, int a_Height) {
	Window* window = getWindowFromGlfwWindow(a_Window);
	window->m_Width = a_Width;
	window->m_Height = a_Height;
}

void Window::windowFramebufferSizeCallback(GLFWwindow * a_Window, int a_Width, int a_Height) {
}

void Window::windowFocusCallback(GLFWwindow * a_Window, int a_Focused) {
}

Window * Window::getWindowFromGlfwWindow(GLFWwindow * a_Window) {
	return m_GlfwToWindowMap.at(a_Window);
}

void Window::applyGlfwWindowDataToClass() {
	m_IsWindowCreated = true;
	glfwGetWindowSize(m_ThisWindow, &m_Width, &m_Height);
	glfwGetFramebufferSize(m_ThisWindow, &m_FrameBufferWidth, &m_FrameBufferHeight);
	m_IsVisable = glfwGetWindowAttrib(m_ThisWindow, GLFW_VISIBLE) != 0;//convert int to bool
}

void Window::checkIfWindowShouldBeMain() {
	if (m_ThisWindow != nullptr) {
		if (m_MainWindow == nullptr) {
			m_MainWindow = this;
		}
	}
}

GLFWwindow * Window::getMainContext() {
	return m_MainWindow == nullptr ? NULL : m_MainWindow->m_ThisWindow;
}
