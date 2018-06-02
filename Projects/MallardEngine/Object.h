#pragma once
#include "DLLBuild.h"
#include "IRenderable.h"

#include "Transform.h"

class DLL_BUILD Object {
public:
	Object(IRenderable* a_Renderable);
	Object(IRenderable* a_Renderable, const char* a_TransformName);
	~Object();

	virtual void update() {};

	IRenderable* m_Renderable = nullptr;

	Transform m_Transform;
};

