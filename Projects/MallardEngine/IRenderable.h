#pragma once
#include "DLLBuild.h"
#include "Transform.h"

enum class VertexRenderingType {
	Normal,
	SkeletalMesh,//todo
	Custom
};

class DLL_BUILD IRenderable {
public:
		DEPRECATED("IRenderable getting removed")
	virtual void draw() = 0;
		DEPRECATED("IRenderable getting removed")
	virtual void drawInstance(unsigned int a_Amount) = 0;

	//Used for sorting when this object renders
	VertexRenderingType m_Type;
};