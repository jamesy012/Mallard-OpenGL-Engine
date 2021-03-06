#pragma once
#include "DLLBuild.h"

#include "..\\IRenderable.h"

class Object;

class RenderMInstanced : public IRenderable {
public:
	RenderMInstanced(IRenderable* a_InstancedRenderable, Object* a_Objects, const int a_MaxDrawAmount);
	~RenderMInstanced();

	int m_DrawAmount = 0;
	IRenderable* m_Renderable;

	// Inherited via IRenderable
	virtual void draw() override;
	virtual void drawInstance(unsigned int a_Amount) override;
};

