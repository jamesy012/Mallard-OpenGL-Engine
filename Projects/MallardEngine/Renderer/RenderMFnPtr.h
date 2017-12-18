#pragma once
#include "DLLBuild.h"

#include <string>

#include "..\\IRenderable.h"

class DLL_BUILD RenderMFnPtr : public IRenderable {
private:
	typedef void(*RendedererFnPtr) (const int a_Amount, IRenderable* a_RenderObject);
public: 
	RenderMFnPtr();
	RenderMFnPtr(RendedererFnPtr a_Function, IRenderable* a_RenderObject = nullptr);
	~RenderMFnPtr();

	// Inherited via IRenderable
	virtual void draw() override;

	virtual void drawInstance(unsigned int a_Amount) override;

	RendedererFnPtr m_Function;
	IRenderable* m_RenderObject
};

