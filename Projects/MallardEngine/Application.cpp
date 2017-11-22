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
#include "Logging.h"

#include "Framebuffer.h"

#include "Mesh.h"
#include "Shader.h"

#include "Skybox.h"

/* assimp include files. These three are usually needed. */
//these are needed for the logging system
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


static Application* m_Application = nullptr;

Application::Application() {
}


Application::~Application() {
	if (m_AppWindow != nullptr) {
		delete m_AppWindow;
		m_AppWindow = nullptr;
	}
}

void Application::run() {

	m_Application = this;

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

	m_AppWindow->m_WindowResizeCallback = windowResize;
	m_AppWindow->m_WindowResizeFramebufferCallback = windowFramebufferResize;

	//set up callbacks for window
	setCallbacksForWindow(m_AppWindow);


	//start glew (opengl)
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		printf("Failed to initialize GLEW\n");
		return;
	}

	/** SET UP PROGRAM FOR STARTUP */

	//set up scene root transform
	m_RootTransform = new Transform("Root Transform");
	Transform::setRootTransform(m_RootTransform);

	Logging::newFrame();

	//set up cameras
	m_GameCamera = new Camera();
	m_GameCamera->setPerspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	m_UiCamera = new Camera();
	m_UiCamera->setOrthographic(0.0f, (float) m_AppWindow->getFramebufferWidth(), 0.0f, (float) m_AppWindow->getFramebufferHeight(), -1000.0f, 1000.0f);


	//gen frame buffers
	{
		const unsigned int NumOfFames = 4;
		Framebuffer** frame[NumOfFames] = { &m_FbGameFrame, &m_FbUIFrame, &m_FbCombinedFrame, &m_FbGameFrameCopy };
		for (int i = 0; i < NumOfFames; i++) {
			(*frame[i]) = new Framebuffer();
			(*frame[i])->setSize(m_AppWindow->getFramebufferWidth(), m_AppWindow->getFramebufferHeight());
			(*frame[i])->createRenderTarget();
		}
	}

	m_PPShader = new Shader();
	m_PPShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
	m_PPShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/PPFrag.frag");
	m_PPShader->linkShader();

	m_PPBcsShader = new Shader();
	m_PPBcsShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
	m_PPBcsShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/PPUber.frag");
	m_PPBcsShader->linkShader();

	m_BasicShader = new Shader();
	m_BasicShader->createSimpleShader(true);

	m_FullScreenQuad = new Mesh();
	m_FullScreenQuad->createPlane(false);

	m_GameSkybox = new Skybox();

	/** SET UP PROGRAM FOR STARTUP LOADING SCREEN */

	//set up default clear color
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

	//todo Add text that will say loading here

	//have the window show something while we load the game
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(m_AppWindow->getWindow());

	//program startup/load models
	startUp();

	/** SET UP PROGRAM FOR RENDERING */
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!m_GameSkybox->hasBeenGenerated()) {
		m_GameSkybox->genSkybox("Textures\\Skybox\\Vindelalven\\");
	}
	if (!m_GameSkybox->hasCameraAssigned()) {
		m_GameSkybox->assignCamera(m_GameCamera);
	}
	Logging::newFrame();

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
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, (std::string("Frame Render: ") + std::to_string(TimeHandler::getCurrentFrameNumber())).c_str());

			if (Input::wasKeyPressed(GLFW_KEY_F)) {
				m_PPBcsShader->reloadShaders();
			}

			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "Update");

			//update the game logic
			update();

			glPopDebugGroup();
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "UI Render");
			//Start UI render
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			//set default framebuffer to be the UI framebuffer
			Framebuffer::setDefaultFramebuffer(m_FbUIFrame);
			Framebuffer::clearCurrentBuffer();

			//set up OpenGL for transparent text
			//glEnable(GL_BLEND);
			//glDisable(GL_DEPTH_TEST);
			//update to ui camera
			m_MainCamera = m_UiCamera;

			//draw the ui
			drawUi();
			
			//end UI Draw
			glPopDebugGroup();
			//we want to include vertices drawn during the main draw
			Logging::objectRenderedAllowAdditions(true);
			//newFrame call is here because:
			//When done before the UI draw, it removes the data that was gathered from the normal draw
			Logging::newFrame();
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 3, -1, "Game Render");
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);

			//Start Game Render
			glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
			//set default framebuffer to be the Game framebuffer
			Framebuffer::setDefaultFramebuffer(m_FbGameFrame);
			Framebuffer::clearCurrentBuffer();

			//render skybox
			m_GameSkybox->draw();

			//update to game camera
			m_MainCamera = m_GameCamera;

			//draw the game
			draw();

			//Start CombinedFrame render
			//disable depth testing
			glDisable(GL_DEPTH_TEST);

			glPopDebugGroup();
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 4, -1, "Final framebuffer Renders");
			//we dont want to include these quad draws in the vertex counts
			Logging::objectRenderedAllowAdditions(false);

			if (Input::wasKeyPressed(GLFW_KEY_U)) {
				m_FbGameFrame->resizeFramebuffer(256, 256);
			}

			if (Input::isKeyDown(GLFW_KEY_G)) {
				Shader::use(m_PPShader);
			} else {
				Shader::use(m_PPBcsShader);
			}
			//set up combined frame framebuffer
			Framebuffer::setDefaultFramebuffer(m_FbCombinedFrame);
			Framebuffer::clearCurrentBuffer();

			//draw the game frame 
			m_FullScreenQuad->setTexture(m_FbGameFrame->getTexture());
			m_FullScreenQuad->draw();

			//set up shader
			Shader::use(m_PPShader);

			//enable blending so the UI doesnt overwrite the game frame
			glEnable(GL_BLEND);

			//draw the UI frame
			m_FullScreenQuad->setTexture(m_FbUIFrame->getTexture());
			m_FullScreenQuad->draw();

			//disable blending
			glDisable(GL_BLEND);

			//set up framebuffer to draw to the backbuffer/screen
			Framebuffer::setDefaultFramebuffer(nullptr);
			Framebuffer::clearCurrentBuffer();

			//and draw the combined frame to the final framebuffer
			m_FullScreenQuad->setTexture(m_FbCombinedFrame->getTexture());
			m_FullScreenQuad->draw();

			//finaly copy framebuffers for next frame
			Framebuffer::framebufferBlit(m_FbGameFrame, m_FbGameFrameCopy);


			glPopDebugGroup();

			glPopDebugGroup();

		}

		glfwSwapBuffers(m_AppWindow->getWindow());
	}

	shutDown();

	//clean up

	delete m_GameCamera;
	delete m_UiCamera;

	delete m_GameSkybox;

	delete m_FbGameFrame;
	delete m_FbGameFrameCopy;
	delete m_FbUIFrame;
	delete m_FbCombinedFrame;

	delete m_FullScreenQuad;
	delete m_PPShader;
	delete m_PPBcsShader;
	delete m_BasicShader;

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

void Application::windowResize(int a_Width, int a_Height) {

}

void Application::windowFramebufferResize(int a_Width, int a_Height) {
	if (m_Application->m_Flags.m_UpdateUICameraToScreenSize) {
		m_Application->m_FbCombinedFrame->setSize(a_Width, a_Height);
		m_Application->m_FbGameFrame->setSize(a_Width, a_Height);
		m_Application->m_FbGameFrameCopy->setSize(a_Width, a_Height);
		m_Application->m_FbUIFrame->setSize(a_Width, a_Height);
	}
	if (m_Application->m_Flags.m_UpdateUICameraToScreenSize) {
		m_Application->m_UiCamera->setOrthographic(0.0f, (float) a_Width, 0.0f, (float) a_Height, -1000.0f, 1000.0f);
	}
}

void Application::checkHandles() {
	if (m_Flags.m_CloseOnEscape) {
		if (Input::wasKeyPressed(GLFW_KEY_ESCAPE)) {
			m_Quit = true;
		}
	}
}

