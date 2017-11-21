#pragma once
#include "DLLBuild.h"

#include <string>


class DLL_BUILD Logging {
public:
	//called when we start a new frame
	static void newFrame();

	static void objectRenderedAllowAdditions(bool a_AllowAdditions);
	//call when rendering a object, a_NumOfVertices is how many
	static void objectRendered(unsigned int a_NumOfVertices);
	//returns the number of rendered Vertices
	static unsigned int getNumVerticesRendered();

	//quick push to stack for timing something
	//requires a quickTimePop after it somewhere
	static void quickTimePush(std::string a_Name, bool a_DisplayToConsole = false);
	//pops last time from stack and calulates the time it took between the last push
	static void quickTimePop(bool a_DisplayToConsole);
};

