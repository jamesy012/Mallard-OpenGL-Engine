#pragma once
#include "DLLBuild.h"

#include "../IRenderable.h"

#include <vector>

class DLL_BUILD RenderMList : public IRenderable {
public:
	// Inherited via IRenderable
	virtual void draw() override;
	virtual void drawInstance(unsigned int a_Amount) override;

	void addObject(IRenderable* a_Renderable);

	std::vector<IRenderable*> m_RenderList;
};

