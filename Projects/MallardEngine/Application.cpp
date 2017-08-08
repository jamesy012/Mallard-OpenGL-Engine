#include "Application.h"

#include <gl\glew.h>
#define _GLFW_BUILD_DLL
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "TimeHandler.h"
#include "Input.h"
#include "ResourceManager.h"
#include "Transform.h"

#include "Window.h"


/* assimp include files. These three are usually needed. */
//these are needed for the logging system
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
}

void Application::run() {

	//start glfw
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

	//create window for app
	m_AppWindow = new Window();
	m_AppWindow->createWindow(640, 480, "Window");
	m_AppWindow->makeContextCurrent();//make context so we can render to it

	//set up callbacks for window
	setCallbacksForWindow(m_AppWindow);


	//start glew (opengl)
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		return;
	}

	//set up scene root transform
	m_RootTransform = new Transform("Root Transform");
	Transform::setRootTransform(m_RootTransform);

	//set up cameras
	m_GameCamera = new Camera();
	m_GameCamera->setPerspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	m_UiCamera = new Camera();
	m_UiCamera->setOrthographic(0.0f, (float)m_AppWindow->getFramebufferWidth(), 0.0f, (float)m_AppWindow->getFramebufferHeight(), -1000.0f, 1000.0f);

	//set up default clear color
	glClearColor(0.75f, 0.0f, 0.75f, 1.0f);

	//todo Add text that will say loading here

	//have the window show something while we load the game
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(m_AppWindow->getWindow());

	//program startup/load models
	startUp();

	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//game loop
	while (!glfwWindowShouldClose(m_AppWindow->getWindow()) && !m_Quit) {
		// Clear the colorbuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
			//update to game camera
			m_MainCamera = m_GameCamera;


			update();

			//clear framebuffer?
			//set up new empty framebuffer

			draw();

			//draw framebuffer

			//draw ui not on the frame buffer


			//Start UI render
			//set up gl property's 
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			//update to ui camera
			m_MainCamera = m_UiCamera;

			//draw the ui
			drawUi();

			//reset gl property's 
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}

		//draw framebuffer

		glfwSwapBuffers(m_AppWindow->getWindow());
	}

	shutDown();

	//clean up

	delete m_GameCamera;
	delete m_UiCamera;
	//remove root transform
	delete m_RootTransform;

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

