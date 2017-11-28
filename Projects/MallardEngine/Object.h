#pragma once
#include "DLLBuild.h"
#include "IRenderable.h"

#include "Transform.h"

class DLL_BUILD Object : public IRenderable {
public:
	Object();
	Object(const char* a_TransformName);
	~Object();

	// Inherited via IRenderable
	virtual void draw() override;
	virtual void drawInstance(unsigned int a_Amount) override;

	virtual void update() {};

	//gets position of this objects transform + IRenderable offset
	glm::vec3 getPositionCombined();
	//returns the transforms global matrix multiplyed by the m_Renderable's global matrix
	glm::mat4 getGlobalMatrixCombined();

	IRenderable* m_Renderable;

};

