#pragma once
#include "DLLBuild.h"

#include <string>

class DLL_BUILD RenderingBase {
public:
	RenderingBase();
	RenderingBase(const char* a_Name);
	virtual ~RenderingBase();

	virtual void fullDraw() {
		preDraw();
		draw();
		postDraw();
	};

protected:
	std::string m_RenderableName = "Rendering: Base";

	virtual void preDraw();
	virtual void draw() = 0;
	virtual void postDraw();
};

struct RendererItems {
public:
	enum Mode {
		Base,
		Renderable
	};

	RendererItems(RenderingBase* a_Object) : m_Mode(Mode::Base), m_Object(a_Object) {}
	RendererItems(class Renderable* a_Object) : m_Mode(Mode::Renderable), m_Object(a_Object) {}

	Mode m_Mode;
	void* m_Object;
};