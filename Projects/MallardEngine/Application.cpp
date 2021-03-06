#include "Application.h"

#include <gl\glew.h>
#define _GLFW_BUILD_DLL
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <time.h>

#include "TimeHandler.h"
#include "Input.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "Transform.h"
#include "Window.h"
#include "Logging.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Font.h"
#include "Skybox.h"

#include "GLDebug.h"

#include "Multithreading/MultithreadManager.h"
#include "Multithreading/MtmThread.h"

/* assimp include files. These three are usually needed. */
//these are needed for the logging system
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Application* Application::m_Application = nullptr;

//true is much slower, but allows the window to be moved during loading
#define RUN_GLFW_ON_OPENGL_THREAD true

Application::Application() {
}


Application::~Application() {
	if (m_ApplicationWindow != nullptr) {
		delete m_ApplicationWindow;
		m_ApplicationWindow = nullptr;
	}
}

void Application::run() {
	srand((unsigned int)time(NULL));

	m_Application = this;

	m_Mtm = new MultithreadManager();
	m_OpenGLThread = m_Mtm->m_OpenGLThread;
	m_LoadingThread = m_Mtm->m_LoadingThread;

#if RUN_GLFW_ON_OPENGL_THREAD
	m_OpenGLThread->queueMethod([](void*) {
#endif
		//start glfw
		if (!glfwInit()) {
			//could not start glfw
			return;
		}


		glfwSetErrorCallback(Application::GLFWErrorCallback);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);//requied Compat profile for Nsight
		//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#if _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // _DEBUG
		{
			Application* ap = m_Application;

			/* get a handle to the predefined STDOUT log stream and attach
			it to the logging system. It remains active for all further
			calls to aiImportFile(Ex) and aiApplyPostProcessing. */
			//struct aiLogStream stream;
			//stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
			//aiAttachLogStream(&stream);

			//create window for app
			ap->m_ApplicationWindow = new Window();
			ap->m_ApplicationWindow->createWindow(640, 480, "Window");
			//m_ApplicationWindow->makeContextCurrent();//make context so we can render to it

			ap->m_ApplicationWindow->m_WindowResizeCallback = windowResize;
			//m_ApplicationWindow->m_WindowResizeFramebufferCallback = windowFramebufferResize;
			ap->m_ApplicationWindow->addFramebufferResize(windowFramebufferResize);

			//set up callbacks for window
			ap->setCallbacksForWindow(ap->m_ApplicationWindow);
		}
#if RUN_GLFW_ON_OPENGL_THREAD
	});
#endif

	m_OpenGLThread->queueMethod([](void*) {
		glfwMakeContextCurrent(Window::getMainWindowGLFW());
	});
	m_OpenGLThread->waitForThread();

	m_OpenGLThread->queueMethod([](void*) {
		//start glew (opengl)
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			printf("Failed to initialize GLEW\n");
			return;
		}

		{
			int glMajor, glMinor;
			const  GLubyte* glVersion;
			const  GLubyte* glVender;
			const  GLubyte* glRenderer;
			glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
			glGetIntegerv(GL_MINOR_VERSION, &glMinor);
			glVersion = glGetString(GL_VERSION);
			glVender = glGetString(GL_VENDOR);
			glRenderer = glGetString(GL_RENDERER);
			printf("OpenGL - %s\n(%i.%i) Vender: %s, Renderer: %s\n\n", glVersion, glMajor, glMinor, glVender, glRenderer);
		}

		/** SET UP PROGRAM FOR STARTUP */
#ifdef _DEBUG
	//other callbacks
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback((GLDEBUGPROC)GLDebug::openGLMessageCallback, 0);
		glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PUSH_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
		glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_POP_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
		//131185 - Buffer detailed info : Buffer object _ID_ (bound to GL_ARRAY_BUFFER_ARB, usage hint is GL_STATIC_DRAW) will use VIDEO memory as the source for buffer object operations.
		const GLsizei numOfIds = 1;
		GLuint ids[numOfIds] = { 131185 };
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, numOfIds, ids, false);
		GLenum error = glGetError();
#endif // _DEBUG

	});
	m_OpenGLThread->waitForThread();

	//set up scene root transform
	m_RootTransform = new Transform("Root Transform");
	Transform::setRootTransform(m_RootTransform);

	Logging::newFrame();



	Texture::m_White1x1Texture = new Texture(1, 1, TextureType::RGB);
	Texture::m_White1x1Texture->setPixel(0, 0, glm::vec4(1, 1, 1, 1));
	m_OpenGLThread->queueMethod([](void*) {
		Texture::m_White1x1Texture->bind();
		GLDebug_NAMEOBJ(GL_TEXTURE, Texture::m_White1x1Texture->getTextureId(), "White 1x1");
	});

	m_OpenGLThread->queueMethod([](void*) {
		Application* ap = m_Application;
		//gen frame buffers
		{
			const unsigned int numOfFames = 4;
			Framebuffer** frame[numOfFames] = { &ap->m_FbGameFrame, &ap->m_FbGameFrameCopy, &ap->m_FbUIFrame, &ap->m_FbCombinedFrame };
			for (int i = 0; i < numOfFames; i++) {
				(*frame[i]) = new Framebuffer();
				//(*frame[i])->setSize(m_ApplicationWindow->getFramebufferWidth(), m_ApplicationWindow->getFramebufferHeight());
				(*frame[i])->setSizePercent(1.0f, 1.0f);
				if (i <= 1) {
					(*frame[i])->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGBA);
					(*frame[i])->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::DEPTH);
					(*frame[i])->genFramebuffer();
				} else if (i >= 2) {
					(*frame[i])->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGBA);
					(*frame[i])->genFramebuffer();
				} else {
					(*frame[i])->createRenderTarget();
				}
			}
		}

		GLDebug_NAMEOBJ(GL_FRAMEBUFFER, ap->m_FbGameFrame->getFramebufferId(), "Game Frame");
		GLDebug_NAMEOBJ(GL_TEXTURE, ap->m_FbGameFrame->getTexture(0)->getTextureId(), "Game Frame - RGBA");
		GLDebug_NAMEOBJ(GL_TEXTURE, ap->m_FbGameFrame->getTexture(1)->getTextureId(), "Game Frame - Depth");
		GLDebug_NAMEOBJ(GL_FRAMEBUFFER, ap->m_FbGameFrameCopy->getFramebufferId(), "Last Game Frame");
		GLDebug_NAMEOBJ(GL_TEXTURE, ap->m_FbGameFrameCopy->getTexture(0)->getTextureId(), "Last Game Frame - RGBA");
		GLDebug_NAMEOBJ(GL_TEXTURE, ap->m_FbGameFrameCopy->getTexture(1)->getTextureId(), "Last Game Frame - Depth");
		GLDebug_NAMEOBJ(GL_FRAMEBUFFER, ap->m_FbUIFrame->getFramebufferId(), "UI Frame");
		GLDebug_NAMEOBJ(GL_TEXTURE, ap->m_FbUIFrame->getTexture(0)->getTextureId(), "UI Frame - RGBA");
		GLDebug_NAMEOBJ(GL_FRAMEBUFFER, ap->m_FbCombinedFrame->getFramebufferId(), "Combined Frame");
		GLDebug_NAMEOBJ(GL_TEXTURE, ap->m_FbCombinedFrame->getTexture(0)->getTextureId(), "Combined Frame - RGBA");
	});

	m_ShaderPPBasic = new Shader();

	m_ShaderPPBcs = new Shader();

	m_ShaderBasic = new Shader();
	m_FullScreenQuad = new Mesh();

	m_OpenGLThread->queueMethod([](void*) {
		Application* ap = m_Application;

		ap->m_ShaderPPBasic->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
		ap->m_ShaderPPBasic->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/PPFrag.frag");
		ap->m_ShaderPPBasic->linkShader();
		ap->m_ShaderPPBcs->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
		ap->m_ShaderPPBcs->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/PPUber.frag");
		ap->m_ShaderPPBcs->linkShader();

		ap->m_ShaderBasic->createSimpleShader(true);

		ap->m_FullScreenQuad->createPlane(false);
	});

	m_SkyboxGame = new Skybox();

	m_Font = new Font();
	//m_Font->loadFont("c:/windows/fonts/comic.ttf", 32);
	m_Font->loadFont("c:/windows/fonts/arial.ttf", 26);

	m_ShaderText = new Shader();

	m_OpenGLThread->queueMethod([](void*) {
		Application* ap = m_Application;
		ap->m_Font->bind();
		Font::generateShaderCode(ap->m_ShaderText);
		ap->m_ShaderText->linkShader();
	});

	/** SET UP PROGRAM FOR STARTUP LOADING SCREEN */

	//set up UI camera
	m_CameraUi = new Camera();
	m_CameraUi->setOrthographic(0.0f, (float)m_ApplicationWindow->getFramebufferWidth(), 0.0f, (float)m_ApplicationWindow->getFramebufferHeight(), -1000.0f, 1000.0f);

	m_OpenGLThread->queueMethod([](void*) {
		//set up default clear color
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	});
	//show loading text before we run startup
	{
		m_OpenGLThread->queueMethod([](void*) {
			Application* ap = m_Application;

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			Shader::use(ap->m_ShaderText);

			ShaderUniformData* uniformPVM = ap->m_ShaderText->m_CommonUniforms.m_ProjectionViewMatrix;
			ShaderUniformData* uniformModel = ap->m_ShaderText->m_CommonUniforms.m_ModelMatrix;

			uniformPVM->setData(&ap->m_CameraUi->getProjectionViewMatrix());
			Shader::applyUniform(uniformPVM);

			Transform model;
			model.setPosition(glm::vec3(0, 100, 0));

			uniformModel->setData(&model);
			Shader::applyUniform(uniformModel);

			ap->m_Font->drawText("--Loading--");

			//have the window show something while we load the game
			glfwSwapBuffers(ap->m_ApplicationWindow->getWindow());
		});
	}


	//** Thread TESTING START
	//std::cout << "Thead testing Starting" << std::endl;

	{
		static bool loaded = false;
		static int value = 0;

		static MtmQueueObj thisObj;
		m_OpenGLThread->queueMethod([](void*) {
			Application* ap = m_Application;
			thisObj = ap->m_OpenGLThread->getCurrentMethod();

			if (!loaded) {
				char text[4] = { '\0' };
				std::fill_n(text, ((value++) % 3) + 1, '.');
				std::string finalString = std::string(text) + "Loading" + std::string(text);
				std::cout << finalString << std::endl;


				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				ap->m_Font->drawText(finalString.c_str());
				glfwSwapBuffers(ap->m_ApplicationWindow->getWindow());
				glfwPollEvents();

				ap->m_OpenGLThread->queueMethod([](void*) {
					Sleep(400);
					MultithreadManager::m_OpenGLThread->queueMethod(thisObj);
				});
				//MultithreadManager::queueMethodThread2([](void*) {
				//	Sleep(400);
				//	MultithreadManager::queueMethod(thisObj);
				//});
			} else {
				glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				ap->m_Font->drawText("This is caused by the skybox loading!");

				glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
				glfwSwapBuffers(ap->m_ApplicationWindow->getWindow());
			}
		});


		Logging::quickTimePush("Program loading", true);
		//program startup/load models
		startUp();

		m_OpenGLThread->queueMethod([](void*) {
			std::cout << "Loading Finished" << std::endl;
		});



		m_LoadingThread->waitForThread();
		loaded = true;
		m_OpenGLThread->waitForThread();
		Logging::quickTimePop(true);

		//remove opengl context from thread because rendering is not multithreaded
		m_OpenGLThread->queueMethod([](void*) {
			glfwMakeContextCurrent(nullptr);
		});
		m_OpenGLThread->waitForThread();
		glfwMakeContextCurrent(Window::getMainWindowGLFW());

	}

	//std::cout << "Thead testing Finished" << std::endl;
	//** Thread TESTING END

	//set up Game camera

	if (m_CameraGame == nullptr) {
		m_CameraGame = new Camera();
	}
	if (m_CameraGame->getProjectionMatrix()[3][3] == 1) {//todo find a better way to check if the camera perspective has been set
		m_CameraGame->setPerspective(60.0f, m_ApplicationWindow->getFramebufferWidth() / (float)m_ApplicationWindow->getFramebufferHeight(), 0.1f, 1000.0f);
	}


	/** SET UP PROGRAM FOR RENDERING */
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	if (!m_SkyboxGame->hasBeenGenerated()) {
		m_SkyboxGame->genSkybox("Textures\\Skybox\\Vindelalven\\");//todo - multithread this
	}
	if (!m_SkyboxGame->hasCameraAssigned()) {
		m_SkyboxGame->assignCamera(m_CameraGame);
	}

	//turns on vsync for 60 fps
	//note without this call Logging::quickTimePush and Logging::quickTimePop appear to get the wrong value,
	//by including the time the program waits for the vsync
	//the interval can be any other number, it just needs this call
	glfwSwapInterval(0);

	Logging::newFrame();

	TimeHandler::update();

	//game loop
	while (!glfwWindowShouldClose(m_ApplicationWindow->getWindow()) && !m_Quit) {
		if (m_Flags.m_RunDebugTimers) {
			//run them every second
			m_DebugRunningTimersThisFrame = (TimeHandler::getCurrentTime() - m_LastDebugTimerRun) > 1;
		} else {
			m_DebugRunningTimersThisFrame = false;
		}
		if (m_DebugRunningTimersThisFrame) {
			//update the last run variable time for the debug timer
			m_LastDebugTimerRun = TimeHandler::getCurrentTime();
			printf("-- Frame %i timing debugs\n", TimeHandler::getCurrentFrameNumber());
			Logging::quickTimePush("Pre Frame");
		}
		//update time
		TimeHandler::update();

		//update old key presses
		Input::update();

#if RUN_GLFW_ON_OPENGL_THREAD
		glfwMakeContextCurrent(nullptr);
		m_OpenGLThread->queueMethod([](void*) {
			//update any new key presses
			glfwMakeContextCurrent(Window::getMainWindowGLFW());
			glfwPollEvents();
			glfwMakeContextCurrent(nullptr);
		});
		m_OpenGLThread->waitForThread();
		glfwMakeContextCurrent(Window::getMainWindowGLFW());
#else
		glfwPollEvents();
#endif

		//check application flags
		checkHandles();

		if (m_DebugRunningTimersThisFrame) {
			Logging::quickTimePop(true);
			Logging::quickTimePush("Frame");
		}

		if (m_Flags.m_CanLockmouse) {
			if (Input::wasMousePressed(0)) {
				m_ApplicationWindow->setMouseLock(true);
				Input::removeMouseButton(0);
			}

			if (m_ApplicationWindow->HasMouseLock()) {
				glfwSetCursorPos(m_ApplicationWindow->getMainWindowGLFW(), 100, 100);
			}
		}


		if (m_Flags.m_RunCameraUpdate) {
			m_CameraGame->update();
		}

		//call virtual functions
		{
			if (m_DebugRunningTimersThisFrame) {
				Logging::quickTimePush("Update");
			}
			Logging::quickGpuDebugGroupPush("Frame Render: " + std::to_string(TimeHandler::getCurrentFrameNumber()));

			if (Input::wasKeyPressed(GLFW_KEY_F)) {
				m_ShaderPPBcs->reloadShaders();
			}

			Logging::quickGpuDebugGroupPush("Update");

			//update the game logic
			update();

			Logging::quickGpuDebugGroupPop();
			if (m_DebugRunningTimersThisFrame) {
				Logging::quickTimePop(true);
				Logging::quickTimePush("Draw");
				Logging::quickTimePush("Draw UI");
			}
			Logging::quickGpuDebugGroupPush("UI Render");
			//Start UI render
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			//set default framebuffer to be the UI framebuffer
			Framebuffer::setDefaultFramebuffer(m_FbUIFrame);
			Framebuffer::clearCurrentBuffer(true, false);

			//set up OpenGL for transparent text
			//glEnable(GL_BLEND);
			//glDisable(GL_DEPTH_TEST);
			//update to ui camera
			m_CameraMain = m_CameraUi;

			//glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			//draw the ui
			drawUi();

			//glDisable(GL_BLEND);

			//end UI Draw
			Logging::quickGpuDebugGroupPop();
			if (m_DebugRunningTimersThisFrame) {
				Logging::quickTimePop(true);
				Logging::quickTimePush("Draw Game");
			}
			//newFrame call is here because:
			//When done before the UI draw, it removes the data that was gathered from the normal draw
			Logging::newFrame();
			Logging::quickGpuDebugGroupPush("Game Render");
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);

			//Start Game Render
			glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
			//set default framebuffer to be the Game framebuffer
			Framebuffer::setDefaultFramebuffer(m_FbGameFrame);
			Framebuffer::clearCurrentBuffer();

			//render skybox
			m_SkyboxGame->draw();

			//we want to include vertices drawn during the main draw
			Logging::objectRenderedAllowAdditions(true);

			//update to game camera
			m_CameraMain = m_CameraGame;

			//draw the game
			draw();

			//Start CombinedFrame render
			//disable depth testing
			glDisable(GL_DEPTH_TEST);

			Logging::quickGpuDebugGroupPop();
			if (m_DebugRunningTimersThisFrame) {
				Logging::quickTimePop(true);
				Logging::quickTimePush("Frame Combine");
			}
			Logging::quickGpuDebugGroupPush("Final framebuffer Renders");
			//we dont want to include these quad draws in the vertex counts
			Logging::objectRenderedAllowAdditions(false);

			if (Input::wasKeyPressed(GLFW_KEY_U)) {
				m_FbGameFrame->setSize(256, 256);
			}

			//set up combined frame framebuffer
			Framebuffer::setDefaultFramebuffer(m_FbCombinedFrame);
			Framebuffer::clearCurrentBuffer();
			//set which shader we are using for drawing the game frame
			if (Input::isKeyDown(GLFW_KEY_G)) {
				Shader::use(m_ShaderPPBasic);
			} else {
				Shader::use(m_ShaderPPBcs);
			}
			Shader::checkUniformChanges();

			//draw the game frame 
			m_FullScreenQuad->setTexture(m_FbGameFrame->getTexture());
			m_FullScreenQuad->draw();

			//set up shader
			Shader::use(m_ShaderPPBasic);
			Shader::checkUniformChanges();

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
			Framebuffer::framebufferBlit(m_FbCombinedFrame, nullptr);

			//finaly copy framebuffers for next frame
			Framebuffer::framebufferBlit(m_FbGameFrame, m_FbGameFrameCopy);


			Logging::quickGpuDebugGroupPop();
			if (m_DebugRunningTimersThisFrame) {
				Logging::quickTimePop(true);
				Logging::quickTimePop(true);
			}
			Logging::quickGpuDebugGroupPop();

		}

		if (m_DebugRunningTimersThisFrame) {
			Logging::quickTimePop(true);
		}

		glfwSwapBuffers(m_ApplicationWindow->getWindow());

	}

	shutDown();

	//clean up

	delete m_CameraGame;
	delete m_CameraUi;

	delete m_SkyboxGame;

	delete m_FbGameFrame;
	delete m_FbGameFrameCopy;
	delete m_FbUIFrame;
	delete m_FbCombinedFrame;

	delete m_FullScreenQuad;
	delete m_ShaderPPBasic;
	delete m_ShaderPPBcs;
	delete m_ShaderBasic;

	delete m_ShaderText;
	delete m_Font;

	delete Texture::m_White1x1Texture;

	//remove root transform
	delete m_RootTransform;

	aiDetachAllLogStreams();
	ResourceManager::deleteLeftOverResources();

	//this deletes all threads
	delete m_Mtm;

	//glfwTerminate
	glfwTerminate();

}


void Application::setCallbacksForWindow(Window * a_Window) {
	a_Window->setCallbacks();

	a_Window->addFramebufferResize(Framebuffer::windowFramebufferResize);

}

void Application::windowResize(int a_Width, int a_Height) {

}

void Application::windowFramebufferResize(int a_Width, int a_Height) {
	if (a_Width == 0 || a_Height == 0) {
		return;
	}
	if (m_Application->m_Flags.m_UpdateUICameraToScreenSize) {
		m_Application->m_CameraUi->setOrthographic(0.0f, (float)a_Width, 0.0f, (float)a_Height, -1000.0f, 1000.0f);
	}
	if (m_Application->m_Flags.m_UpdateGameCameraToScreenSize) {
		m_Application->m_CameraGame->setPerspective(60.0f, a_Width / (float)a_Height, 0.1f, 1000.0f);
	}
}

void Application::checkHandles() {
	if (Input::wasKeyPressed(GLFW_KEY_ESCAPE)) {
		if (m_Flags.m_CanLockmouse && m_ApplicationWindow->HasMouseLock()) {
			m_ApplicationWindow->setMouseLock(false);
			Input::removeKeyButton(GLFW_KEY_ESCAPE);
		} else {
			if (m_Flags.m_CloseOnEscape) {
				m_Quit = true;
			}
		}
	}
}

void Application::GLFWErrorCallback(int a_ErrorCode, const char * a_Description) {
	std::cout << "GLFW ERROR _" << a_ErrorCode << "_" << std::endl;
	std::cout << "\t GLFW_" << a_Description << "_" << std::endl;
}

