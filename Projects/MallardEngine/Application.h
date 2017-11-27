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

class DLL_BUILD Application {
public:
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

	//bool to store if application is printing out it's timer this frame
	bool m_DebugRunTimers = false;

	//flags/handles
	struct Flags {
		bool m_CloseOnEscape = true;
		bool m_UpdateUICameraToScreenSize = true;
		bool m_UpdateGameCameraToScreenSize = true;
		bool m_AllowInternalFramebufferResizes = true;
		bool m_RunDebugTimers = true;
	} m_Flags;

	Window* m_ApplicationWindow = nullptr;

	//reference to the main camera in the scene
	//Main Camera will be set to m_GameCamera at the start of draw()
	//and set to m_UiCamera at the start of drawUi()
	Camera* m_CameraMain;
	//Game Camera and UI Camera
	Camera* m_CameraGame = nullptr;
	Camera* m_CameraUi = nullptr;

	//
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

	Shader* m_ShaderText;
	Font* m_Font;
private:
	void setCallbacksForWindow(Window* a_Window);
	static void windowResize(int a_Width, int a_Height);
	static void windowFramebufferResize(int a_Width, int a_Height);

	void checkHandles();

	//root transform for every transform
	Transform* m_RootTransform;


};

