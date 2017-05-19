#include "Application.h"

#define _GLFW_BUILD_DLL
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "TimeHandler.h"
#include "Input.h"

#include "Window.h"

Application::Application() {
}


Application::~Application() {
	if (m_AppWindow != nullptr) {
		delete m_AppWindow;
		m_AppWindow = nullptr;
	}
}

void Application::run() {

	if (!glfwInit()) {
		//could not start glfw
		return;
	}

	//create window
	m_AppWindow = new Window();
	if (!m_AppWindow->createWindow(640, 480, "Window")) {
		printf("NO WINDOW!?\n");
		// Window or OpenGL context creation failed
	}

	setCallbacksForWindow(m_AppWindow->getWindow());
	m_AppWindow->setCallbacks();//<- move to function above

	startUp();

	//game loop
	while (!glfwWindowShouldClose(m_AppWindow->getWindow()) && !m_Quit) {
		//update time
		TimeHandler::update();

		//update old key presses
		Input::update();

		//update any new key presses
		glfwPollEvents();

		//check application flags
		checkHandles();

		//call virtual functions
		{
			update();

			draw();

			//change camera?
			drawUi();
		}

		glfwSwapBuffers(m_AppWindow->getWindow());
	}

	shutDown();

	//glfwTerminate
	glfwTerminate();
}


void Application::setCallbacksForWindow(GLFWwindow * a_Window) {
	glfwSetKeyCallback(a_Window, Input::keyCallback);
	glfwSetCursorPosCallback(a_Window, Input::cursorPositionCallback);
	glfwSetMouseButtonCallback(a_Window, Input::mouseButtonCallback);
	glfwSetCharCallback(a_Window, Input::charCallback);
	glfwSetScrollCallback(a_Window, Input::scrollCallback);
	
}

void Application::checkHandles() {
	if (m_CloseOnEscape) {
		if (Input::wasKeyPressed(GLFW_KEY_ESCAPE)) {
			m_Quit = true;
		}
	}
}

