#pragma once
#include "DLLBuild.h"
#include "RenderingBase.h"
#include "RenderingHolder.h"

#include <vector>

class RenderingObject;
class Shader;

class DLL_BUILD RenderingList : public RenderingBase {
public:
	RenderingList();
	virtual ~RenderingList();

	virtual void draw() override;

	std::vector<RenderingHolder*> m_Objects;
	std::vector<RenderingHolder*> m_TempObjects;

	Shader* m_DefaultShader;


	void CreateTempRenderingHolder(class Object* a_Object);
};
