#pragma once
#include "DLLBuild.h"

#include "Renderer/Renderable.h"

#include "Transform.h"

class DLL_BUILD Object {
public:
	Object(Renderable* a_Renderable);
	Object(Renderable* a_Renderable, const char* a_TransformName);
	~Object();

	virtual void update() {};

	Renderable* m_Renderable = nullptr;

	Transform m_Transform;
};

