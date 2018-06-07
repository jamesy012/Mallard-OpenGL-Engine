#pragma once

#include "Application.h"

class Model;
class Mesh;
class Text;

class DepthOfField;
class RenderMList;
class Object;
class MeshBatch;

#include <btBulletDynamicsCommon.h>

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

	bool m_RenderDOF = false;

	bool m_AutoDepth = false;
	float m_CurrentDepth = 0;
	const float m_MaxDist = 35.0f;

	bool m_FocalDistanceDirection = true;
	bool m_RunningFocalDistanceChange = false;
	float m_FocalDistanceTimer = 0.0f;


	/// - physics
	Mesh* m_SphereModel;
	Object* m_SphereObject[2];

	// - bullet objects
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfig;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	//groundPlane (Test Object)
	btCollisionShape* groundPlane;
	btDefaultMotionState* groundPlaneMotionState;
	btRigidBody* groundPlaneRigidBody;

	btCollisionShape* fallSphere;
	btDefaultMotionState* fallSphereMotionState;
	btRigidBody* fallSphereRigidBody;

	btDefaultMotionState* groundSphereMotionState;
	btRigidBody* groundSphereRigidBody;
};

