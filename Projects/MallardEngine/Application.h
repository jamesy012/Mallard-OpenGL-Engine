#pragma once
#include "DLLBuild.h"

struct GLFWwindow;

class Application {
public:
	DLL_BUILD Application();
	DLL_BUILD ~Application();

	DLL_BUILD void run();

protected:
	DLL_BUILD virtual void startUp() = 0;
	DLL_BUILD virtual void shutDown() = 0;
	DLL_BUILD virtual void update() = 0;
	DLL_BUILD virtual void draw() = 0;
	DLL_BUILD virtual void drawUi() = 0;

	//application related stuff
	bool m_Quit = false;

	//flags/handles
	bool m_CloseOnEscape = true;

private:
	void setCallbacksForWindow(GLFWwindow* a_Window);

	void checkHandles();
};

