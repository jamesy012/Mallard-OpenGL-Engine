#pragma once
#include "DLLBuild.h"

#include "Camera.h"

class Window;
class Transform;

class Framebuffer;
class Mesh;
class Shader;
class Skybox;
class Font;

class MultithreadManager;
class MtmThread;


class DLL_BUILD Application {
public:
	static Application* m_Application;

	Application();
	~Application();

	//main starting point for the program,
	//will open a window and start opengl
	//calls startUp/shutDown/update and draw 
	void run();

protected:
	virtual void startUp() = 0;
	virtual void shutDown() = 0;
	virtual void update() = 0;
	virtual void draw() = 0;
	virtual void drawUi() = 0;

	//application related stuff
	bool m_Quit = false;

	//flags/handles
	struct Flags {
		bool m_CloseOnEscape = true;
		bool m_UpdateUICameraToScreenSize = true;
		bool m_UpdateGameCameraToScreenSize = true;
		//when the window is resized should we also resize the framebuffers
		bool m_AllowInternalFramebufferResizes = true;
		//allow the debug timer to run every second, when it does run m_DebugRunningTimersThisFrame will be true
		bool m_RunDebugTimers = false;
		bool m_RunCameraUpdate = true;
		//should we lock the mouse or not?
		bool m_CanLockmouse = false;
	} m_Flags;

	Window* m_ApplicationWindow = nullptr;

	//reference to the main camera in the scene
	//Main Camera will be set to m_GameCamera at the start of draw()
	//and set to m_UiCamera at the start of drawUi()
	Camera* m_CameraMain;
	//Game Camera and UI Camera
	Camera* m_CameraGame = nullptr;
	Camera* m_CameraUi = nullptr;

	//framebuffers for game, ui and rendering to the screen render
	Framebuffer* m_FbGameFrame;
	Framebuffer* m_FbGameFrameCopy;
	Framebuffer* m_FbUIFrame;
	Framebuffer* m_FbCombinedFrame;

	Skybox* m_SkyboxGame = nullptr;

	Mesh* m_FullScreenQuad;
	//Basic post postprocessing
	Shader* m_ShaderPPBasic;
	//Post postprocessing brightness, contrast, saturation shader
	Shader* m_ShaderPPBcs;
	Shader* m_ShaderBasic;

	//shader for rendering text
	Shader* m_ShaderText;
	//reference to a precreated font object
	Font* m_Font;

	MultithreadManager* m_Mtm;
	MtmThread* m_OpenGLThread;
	MtmThread* m_LoadingThread;

	//bool to store if application is printing out it's timer this frame
	bool m_DebugRunningTimersThisFrame = false;

private:
	void setCallbacksForWindow(Window* a_Window);
	static void windowResize(int a_Width, int a_Height);
	static void windowFramebufferResize(int a_Width, int a_Height);

	void checkHandles();

	static void GLFWErrorCallback(int a_ErrorCode, const char* a_Description);

	//root transform for every transform
	Transform* m_RootTransform;

	float m_LastDebugTimerRun = 0;

public:
	Flags getFlags() {
		return m_Flags;
	}
	Camera* getCurrentCamera() {
		return m_CameraMain;
	}
};

