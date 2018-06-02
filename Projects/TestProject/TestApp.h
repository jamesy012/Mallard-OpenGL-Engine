#pragma once

#include "Application.h"

class Model;
class Mesh;
class Text;

class DepthOfField;
class RenderMList;
class Object;
class MeshBatch;

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
	Model* m_Ground;
	Model* m_GrassModel;
	MeshBatch* m_GrassBatch;

	Object* m_ModelObject;
	Object* m_GroundObject;
	Object* m_GrassBatchObject;

	Text* m_TestText;

	DepthOfField* m_DOFTest;
	RenderMList* m_RenderList;

	bool m_RenderDOF = true;

	bool m_FocalDistanceDirection = true;
	bool m_RunningFocalDistanceChange = false;
	float m_FocalDistanceTimer = 0.0f;
};

