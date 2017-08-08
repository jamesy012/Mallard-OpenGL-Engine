#pragma once

#include "DLLBuild.h"

class Application;

class DLL_BUILD TimeHandler {
	friend Application;
public:

	//returns the current frame number
	static unsigned int getCurrentFrameNumber();
	//gets the current time since the start of the program
	static float getCurrentTime();
	//gets the time of the previous frame
	static float getPreviousTime();
	//gets the scaled delta time
	//this can go above the max delta time if the scale is large
	static float getDeltaTime();
	//gets a unscaled version of delta time
	//equivalent to a delta time of 1
	static float getUnscaledDeltaTime();
	//gets the current scale of deltaTime
	static float getDeltaTimeScale();

	//returns the fps of the program
	static unsigned int getFps();

	//sets the scale for the deltaTime
	//this does not effect time
	static void setDeltaTimeScale(float a_Scale);

	//sets the max possible delta time that is allowed
	//this is to prevent a massive delta time teleporting objects
	static void setMaxDeltaTime(float a_MaxDt);
protected:
	static void update();

	//static variables for deltaTime are in .cpp
};
