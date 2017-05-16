#pragma once

#include "DLLBuild.h"

class Application;

class TimeHandler {
	friend Application;
public:

	DLL_BUILD static int getCurrentFrameNumber();
	DLL_BUILD static float getCurrentTime();
	DLL_BUILD static float getPreviousTime();
	DLL_BUILD static float getDeltaTime();
	DLL_BUILD static float getUnscaledDeltaTime();
	DLL_BUILD static float getDeltaTimeScale();

	DLL_BUILD static void setDeltaTimeScale(float a_Scale);

	DLL_BUILD static void setMaxDeltaTime(float a_MaxDt);
protected:
	static void update();

	//static variables for deltaTime are in .cpp
};
