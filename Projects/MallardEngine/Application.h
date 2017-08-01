#pragma once
#include "DLLBuild.h"

#include "Camera.h"

class Window;
class Transform;

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
	bool m_CloseOnEscape = true;

	Window* m_AppWindow = nullptr;

	//reference to the main camera in the scene
	Camera* m_MainCamera;
	//Game Camera and UI Camera
	Camera* m_GameCamera = nullptr;
	Camera* m_UiCamera = nullptr;
private:
	void setCallbacksForWindow(Window* a_Window);

	void checkHandles();

	//root transform for every transform
	Transform* m_RootTransform;


};

