#pragma once

#include "DLLBuild.h"

#include <vector>

class Object;
class Shader;
class Framebuffer;

class DLL_BUILD ObjectHighlight {
public:
	ObjectHighlight();
	~ObjectHighlight();

	std::vector<Object*> m_HighlightList;

	void create();

	void use();

private:
	Shader* m_Highlightshader;
	Shader* m_BlurShader;
	Framebuffer* m_Framebuffer;
	Framebuffer* m_BlurBuffer;

};
