#pragma once



#include "DLLBuild.h"

class Application {
public:
	DLL_BUILD Application();
	DLL_BUILD ~Application();

	DLL_BUILD float getRandom();
};

