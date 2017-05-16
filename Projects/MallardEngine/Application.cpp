#include "Application.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "TimeHandler.h"
#include "Input.h"

Application::Application() {
}


Application::~Application() {
}

void Application::run() {

	if (!glfwInit()) {
		//could not start glfw
		return;
	}

	//create window
	GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
	if (!window) {
		printf("NO WINDOW!?\n");
		// Window or OpenGL context creation failed
	}

	setCallbacksForWindow(window);

	startUp();

	//game loop
	while (!glfwWindowShouldClose(window) && !m_Quit) {
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

			//change camera!
			drawUi();
		}

		glfwSwapBuffers(window);
	}

	shutDown();

	//glfwTerminate
	glfwDestroyWindow(window);
	glfwTerminate();
}


void Application::setCallbacksForWindow(GLFWwindow * a_Window) {
	glfwSetKeyCallback(a_Window, Input::keyCallback);
	glfwSetCursorPosCallback(a_Window, Input::cursorPositionCallback);
	glfwSetMouseButtonCallback(a_Window, Input::mouseButtonCallback);
	glfwSetCharCallback(a_Window, Input::charCallback);
	glfwSetScrollCallback(a_Window, Input::scrollCallback);
	//glfwSetWindowSizeCallback(window, Window::windowSizeCallback);
	//glfwSetFramebufferSizeCallback(window, Window::framebufferSizeCallback);
	//glfwSetWindowFocusCallback(window, Window::windowFocusCallback);
}

void Application::checkHandles() {
	if (m_CloseOnEscape) {
		if (Input::wasKeyPressed(GLFW_KEY_ESCAPE)) {
			m_Quit = true;
		}
	}
}

