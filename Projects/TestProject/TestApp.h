#pragma once

#include "Application.h"

class Model;
class Mesh;
class Text;

#include "PostEffects/DepthOfField.h"
#include "Renderer/RenderMList.h"

class  TestApp :
	public Application {
public:
	// Inherited via Application
	virtual void startUp() override;
	virtual void shutDown() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void drawUi() override;

private:

	Model* m_Model;

	//small quad
	Mesh* m_QuadMesh;

	Text* m_TestText;

	DepthOfField m_DOFTest;
	RenderMList m_RenderList;
};

