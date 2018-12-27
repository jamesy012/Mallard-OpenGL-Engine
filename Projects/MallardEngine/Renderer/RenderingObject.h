#pragma once
#include "DLLBuild.h"
#include "RenderingBase.h"
#include "Renderable.h"


class DLL_BUILD RenderingObject : public RenderingBase {
public:
	RenderingObject();
	virtual ~RenderingObject();

	class Object* m_Object;

protected:
	virtual void draw() override;
};

