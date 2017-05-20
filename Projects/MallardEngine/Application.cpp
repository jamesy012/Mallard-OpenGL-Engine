#include "Application.h"

#define _GLFW_BUILD_DLL
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "TimeHandler.h"
#include "Input.h"

#include "Window.h"

/* assimp include files. These three are usually needed. */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Application::Application() {
}


Application::~Application() {
	if (m_AppWindow != nullptr) {
		delete m_AppWindow;
		m_AppWindow = nullptr;
	}
	if (m_ContextWindow != nullptr) {
		delete m_ContextWindow;
		m_ContextWindow = nullptr;
	}
}

void Application::run() {

	if (!glfwInit()) {
		//could not start glfw
		return;
	}

	/* get a handle to the predefined STDOUT log stream and attach
	it to the logging system. It remains active for all further
	calls to aiImportFile(Ex) and aiApplyPostProcessing. */
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
	aiAttachLogStream(&stream);

	m_ContextWindow = new Window();
	m_AppWindow = new Window();

	m_ContextWindow->createWindow();//create an empty context

	startUp();

	if (!m_AppWindow->isWindowCreated()) {
		//create window
		if (!m_AppWindow->createWindow(640, 480, "Window")) {
			printf("NO WINDOW!?\n");
			// Window or OpenGL context creation failed
		}
	}

	setCallbacksForWindow(m_AppWindow);


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


void Application::setCallbacksForWindow(Window * a_Window) {
	a_Window->setCallbacks();

}

void Application::checkHandles() {
	if (m_CloseOnEscape) {
		if (Input::wasKeyPressed(GLFW_KEY_ESCAPE)) {
			m_Quit = true;
		}
	}
}

