#pragma once
#include "DLLBuild.h"

#include <string>

#include "..\\IRenderable.h"

class DLL_BUILD RenderMFnPtr : public IRenderable {
private:
	typedef void(*RendedererFnPtr) ();
public:
	RenderMFnPtr();
	RenderMFnPtr(RendedererFnPtr a_Function);
	~RenderMFnPtr();

	// Inherited via IRenderable
	virtual void draw() override;

	virtual void drawInstance(unsigned int a_Amount) override;

	RendedererFnPtr m_Function;
};

