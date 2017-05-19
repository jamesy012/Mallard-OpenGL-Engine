#pragma once
#include "DLLBuild.h"

class Window;

struct GLFWwindow;

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
private:
	void setCallbacksForWindow(GLFWwindow* a_Window);

	void checkHandles();

};

