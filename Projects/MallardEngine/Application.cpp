#include "Application.h"

#include <gl\glew.h>
#define _GLFW_BUILD_DLL
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "TimeHandler.h"
#include "Input.h"
#include "ResourceManager.h"

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
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* get a handle to the predefined STDOUT log stream and attach
	it to the logging system. It remains active for all further
	calls to aiImportFile(Ex) and aiApplyPostProcessing. */
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
	aiAttachLogStream(&stream);

	m_ContextWindow = new Window();
	m_ContextWindow->createWindow();//create an empty context
	m_ContextWindow->makeContextCurrent();//make context so glew can load

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		return;
	}

	m_AppWindow = new Window();

	if (!m_AppWindow->isWindowCreated()) {
		//create window
		if (!m_AppWindow->createWindow(640, 480, "Window")) {
			printf("NO WINDOW!?\n");
			// Window or OpenGL context creation failed
		}
	}
	m_AppWindow->makeContextCurrent();//make context so we can render to it

	//startUp();//moved to the first run check in loop





	setCallbacksForWindow(m_AppWindow);

	glClearColor(0.75f, 0.0f, 0.75f, 1.0f);
	bool firstRun = true;

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
		if(!firstRun){
			update();

			draw();

			//change camera?
			drawUi();
		} else {
			startUp();
			firstRun = false;
		}


		glfwSwapBuffers(m_AppWindow->getWindow());
		// Clear the colorbuffer
		glClear(GL_COLOR_BUFFER_BIT);
	}

	shutDown();

	aiDetachAllLogStreams();
	ResourceManager::deleteLeftOverResources();

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

