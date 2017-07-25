#pragma once

#include "DLLBuild.h"

class Application;

class DLL_BUILD TimeHandler {
	friend Application;
public:

	static unsigned int getCurrentFrameNumber();
	static float getCurrentTime();
	static float getPreviousTime();
	static float getDeltaTime();
	static float getUnscaledDeltaTime();
	static float getDeltaTimeScale();

	static void setDeltaTimeScale(float a_Scale);

	static void setMaxDeltaTime(float a_MaxDt);
protected:
	static void update();

	//static variables for deltaTime are in .cpp
};
