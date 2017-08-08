#pragma once
#include "DLLBuild.h"
#include "Transform.h"

class DLL_BUILD Object {
public:
	Object();
	Object(const char* a_TransformName);
	~Object();

	Transform m_Transform;
};

