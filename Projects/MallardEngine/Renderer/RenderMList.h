#pragma once
#include "DLLBuild.h"

#include "../IRenderable.h"
class Object;

#include <vector>


class DLL_BUILD RenderMList : public IRenderable {
public:
	// Inherited via IRenderable
	virtual void draw() override;
	virtual void drawInstance(unsigned int a_Amount) override;

	void addObject(Object* a_Renderable);

	std::vector<Object*> m_RenderList;
};

