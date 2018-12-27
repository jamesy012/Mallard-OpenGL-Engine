#pragma once
#include "DLLBuild.h"

class RenderingBase;
class Shader;

class DLL_BUILD RenderingHolder {
public:
	//Which rendering block does this belong to?
	RenderingBase * m_Renderer = nullptr;
	//what shader should we be using?
	Shader * m_Shader = nullptr;

};