#pragma once
#include "DLLBuild.h"

#include "Camera.h"

class Window;
class Transform;

class Framebuffer;
class Mesh;
class Shader;

class DLL_BUILD Application {
public:
	Application();
	~Application();

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
	} m_Flags;

	Window* m_AppWindow = nullptr;

	//reference to the main camera in the scene
	//Main Camera will be set to m_GameCamera at the start of draw()
	//and set to m_UiCamera at the start of drawUi()
	Camera* m_MainCamera;
	//Game Camera and UI Camera
	Camera* m_GameCamera = nullptr;
	Camera* m_UiCamera = nullptr;

	//
	Framebuffer* m_FbGameFrame;
	Framebuffer* m_FbGameFrameCopy;
	Framebuffer* m_FbUIFrame;
	Framebuffer* m_FbCombinedFrame;

	Mesh* m_FullScreenQuad;
	Shader* m_PPShader;
	Shader* m_BasicShader;
private:
	void setCallbacksForWindow(Window* a_Window);
	static void windowResize(int a_Width, int a_Height);

	void checkHandles();

	//root transform for every transform
	Transform* m_RootTransform;



};

