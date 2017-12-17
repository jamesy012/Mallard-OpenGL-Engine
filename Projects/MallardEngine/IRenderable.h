#pragma once
#include "DLLBuild.h"
#include "Transform.h"

class DLL_BUILD IRenderable {
public:
	virtual void draw() = 0;
	virtual void drawInstance(unsigned int a_Amount) = 0;

	virtual glm::mat4 getTransformMatrix() {
		return m_Transform.getGlobalMatrix();
	}
	//todo - move this to a IRenderableTransform like class

	Transform m_Transform;
};