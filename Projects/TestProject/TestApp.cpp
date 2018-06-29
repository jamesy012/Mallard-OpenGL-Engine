#include "TestApp.h"

#include <stdio.h>

#include <gl\glew.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "Logging.h"

#include "TimeHandler.h"
#include "Window.h"
#include "Transform.h"
#include "Input.h"
#include "Keys.h"

#include "Model.h"
#include "Mesh.h"
#include "MeshBatch.h"
#include "Renderer/RenderMList.h"

#include "Font.h"
#include "Text.h"

#include "Framebuffer.h"
#include "Texture.h"
#include "Shader.h"
#include "PostEffects/DepthOfField.h"

#include "Object.h"
#include "Camera.h"
#include "CameraFly.h"

#include "Multithreading/MtmThread.h"
#include <BulletCollision/CollisionShapes/btStridingMeshInterface.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include "Conversions.h"

void TestApp::startUp() {

	m_TestText = new Text(m_Font);
	//m_TestText->generateText(R"(TEST APP
	//	Move: WASDQE, Arrow keys
	//	Text Size/[Color = 255, 0 ,0]Color [Color=128,64,64,128]Test:[Color = ]
	//	[Size=25]B[Size=32]B Size test![Size=28]![Size=22]![Size=18]![Size=14]!)", 38, glm::vec4(72, 190, 32, 255) / 255.0f);
	m_TestText->generateText(R"(TEST APP
		Move: WASDQE, Arrow keys)", 20);
	});

	m_CameraGame = new CameraFly();
	m_CameraGame->m_Transform.setPosition(glm::vec3(-5.842, 7.482, 1.879));
	m_CameraGame->m_Transform.setRotation(glm::vec3(40.6868, -74.030, 0));
	((CameraFly*)m_CameraGame)->m_KeyboardKeys.MoveUp = KEY_E;
	((CameraFly*)m_CameraGame)->m_KeyboardKeys.MoveDown = KEY_Q;
	((CameraFly*)m_CameraGame)->m_RotationMode |= CameraFly::RotationModes::MOUSE;

	m_DOFTest = new DepthOfField();
	m_OpenGLThread->queueMethod(this, [](void* tp) {
		((TestApp*)tp)->m_DOFTest->create();
	});

	m_Model = new Model();
	m_LoadingThread->queueMethod(this, [](void* tp) {
		((TestApp*)tp)->m_Model->load("Models/Nanosuit/nanosuit.obj");
	});
	m_Ground = new Model();
	m_Ground->load("Models/test/Ground.obj");
	m_GrassModel = new Model();
	m_GrassModel->load("Models/test/GrassPack/Grass_02.obj");

	printf("grass gen Start\n");
	m_GrassBatch = new MeshBatch();
	if (false) {
		const unsigned int NumOfGrass = 2000;
		const unsigned int sizeOfGround = 50;
		for (int i = 0; i < NumOfGrass; i++) {
			glm::mat4 position;
			glm::vec3 pos;// = m_Ground->m_Meshs[0]->getVertexPosition(i);

			pos.x = (((rand() % 10000) / 10000.0f) * (sizeOfGround * 2)) - sizeOfGround;
			pos.y = 1;
			pos.z = (((rand() % 10000) / 10000.0f) * (sizeOfGround * 2)) - sizeOfGround;

			//printf("Pos %i (%f,%f,%f) ---- ", i, pos.x, pos.y, pos.z);
			glm::vec3 height = m_Ground->m_Meshs[0]->getClosestPosition(pos);
			pos.y = height.y;
			//pos = height;
			//printf("Pos %i (%f,%f,%f)\n", i, pos.x, pos.y, pos.z);
			//const glm::vec3 posOnGround = glm::vec3(i,0,0);
			position = glm::translate(glm::mat4(1), pos);
			position = glm::rotate(position, ((rand() % 10000) / 10000.0f) * 360, glm::vec3(0, 1, 0));
			position = glm::scale(position, glm::vec3(((rand() % 10000) / 10000.0f) * 3));
			m_GrassBatch->add(m_GrassModel, position);
		}
		m_GrassBatch->bind();
	}
	printf("grass gen Finished\n");

	m_ModelObject = new Object(m_Model);
	m_GroundObject = new Object(m_Ground);
	m_GrassBatchObject = new Object(m_GrassBatch);

	m_RenderList = new RenderMList();
	m_RenderList->addObject(m_ModelObject);
	m_RenderList->addObject(m_GroundObject);
	m_RenderList->addObject(m_GrassBatchObject);


	//setup physics
	broadphase = new btDbvtBroadphase();
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	//objects
	m_SphereModel = new Mesh();
	m_OpenGLThread->queueMethod(this, [](void* tp) {
		((TestApp*)tp)->m_SphereModel->createBox();
	});
	m_SphereObject[0] = new Object(m_SphereModel, "Physics Object 1!");
	m_SphereObject[1] = new Object(m_SphereModel, "Physics Object 2!");

	m_RenderList->addObject(m_SphereObject[0]);
	m_RenderList->addObject(m_SphereObject[1]);

	groundPlane = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

	groundPlaneMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundPlaneRigidBodyCI(0, groundPlaneMotionState, groundPlane, btVector3(0, 0, 0));
	groundPlaneRigidBodyCI.m_restitution = 0.75f;
	groundPlaneRigidBody = new btRigidBody(groundPlaneRigidBodyCI);

	//fallSphere = new btSphereShape(1);
	fallSphere = new btBoxShape(btVector3(1, 1, 1));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallSphere->calculateLocalInertia(mass, fallInertia);

	fallSphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 1, 0)));
	btRigidBody::btRigidBodyConstructionInfo fallSphereRigidBodyCI(mass, fallSphereMotionState, fallSphere, fallInertia);
	fallSphereRigidBodyCI.m_restitution = 0.75f;

	fallSphereRigidBody = new btRigidBody(fallSphereRigidBodyCI);
	fallSphereRigidBody->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));

	groundSphereRigidBody = new btRigidBody(fallSphereRigidBodyCI);
	groundSphereRigidBody->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(1.5f, 1, 0)));

	//dynamicsWorld->addRigidBody(groundPlaneRigidBody);
	dynamicsWorld->addRigidBody(fallSphereRigidBody);
	dynamicsWorld->addRigidBody(groundSphereRigidBody);

	{
		//memory leak's everywhere!
		btTriangleMesh* mesh = new btTriangleMesh();
		mesh->preallocateIndices(m_Ground->m_Meshs[0]->m_Indices.size());
		mesh->preallocateVertices(m_Ground->m_Meshs[0]->m_Vertices.size());
		for (unsigned int i = 0; i < m_Ground->m_Meshs[0]->m_Indices.size(); i += 3) {
			glm::vec3 pos0 = m_Ground->m_Meshs[0]->m_Vertices[m_Ground->m_Meshs[0]->m_Indices[i + 0]].position;
			glm::vec3 pos1 = m_Ground->m_Meshs[0]->m_Vertices[m_Ground->m_Meshs[0]->m_Indices[i + 1]].position;
			glm::vec3 pos2 = m_Ground->m_Meshs[0]->m_Vertices[m_Ground->m_Meshs[0]->m_Indices[i + 2]].position;
			mesh->addTriangle(
				btVector3(pos0.x, pos0.y, pos0.z),
				btVector3(pos1.x, pos1.y, pos1.z),
				btVector3(pos2.x, pos2.y, pos2.z));
		}

		//btCollisionShape* groundMesh = new btTriangleMeshShape(smi);
		btCollisionShape* groundMesh = new btBvhTriangleMeshShape(mesh, true);

		//btCollisionShape* groundMesh = new btConvexHullShape(points[0].m_floats, points.size(),16);
		btDefaultMotionState* groundMeshMS = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
		btRigidBody::btRigidBodyConstructionInfo groundMeshRBCI(0, groundMeshMS, groundMesh, btVector3(0, 0, 0));
		groundMeshRBCI.m_restitution = 0.75f;
		btRigidBody* groundMeshRB = new btRigidBody(groundMeshRBCI);

		dynamicsWorld->addRigidBody(groundMeshRB);
	}
	

}

void TestApp::shutDown() {
	m_Model->unload();
	m_Ground->unload();
	m_GrassModel->unload();

	delete m_GrassBatch;
	delete m_GrassBatchObject;

	delete m_ModelObject;
	delete m_GroundObject;
	delete m_DOFTest;
	delete m_RenderList;

	delete m_TestText;
}

void TestApp::update() {
	if (Input::wasKeyPressed(KEY_F1)) {
		m_Flags.m_RunDebugTimers ^= 1;
	}

	if (Input::wasKeyPressed(KEY_KP_1)) {
		m_RenderDOF ^= 1;
	}


	if (Input::wasKeyPressed(KEY_T)) {
		static int val = 0;
		static const float values[] = { 2.0f,20.0f,0,100.0f };
		m_DOFTest->setValue("falloff", values[(val++) % 4]);

	}

	if (Input::wasKeyPressed(KEY_KP_2)) {
		m_AutoDepth ^= 1;
		m_RunningFocalDistanceChange = true;
	}

	if (m_AutoDepth) {
		//run 3 times a second, because it's slow
		if (TimeHandler::getCurrentFrameNumber() % 20 == 0) {
			printf("update\n");
			m_CurrentDepth = m_FbGameFrameCopy->getDepthAtPoint(0.5f, 0.5f);
		}
		if (m_CurrentDepth > m_MaxDist) {
			float falloffVal = glm::lerp(m_DOFTest->getValue("falloff"), 0.0f, TimeHandler::getDeltaTime() * 5);
			m_DOFTest->setValue("falloff", falloffVal);
		} else {
			float percentage = m_CurrentDepth / m_MaxDist;
			percentage = -glm::log(percentage) / 10.5f;
			percentage = glm::clamp(percentage, 0.0f, 1.0f);
			//printf("%f\n", percentage*20.0f);
			float falloffVal = glm::lerp(m_DOFTest->getValue("falloff"), percentage * 20.0f, TimeHandler::getDeltaTime() * 3);
			m_DOFTest->setValue("falloff", falloffVal);
			float setVal = glm::lerp(m_DOFTest->getValue("focusDistance"), m_CurrentDepth, TimeHandler::getDeltaTime() * 3);
			m_DOFTest->setValue("focusDistance", setVal);
		}
	} else {

		if (Input::wasKeyPressed(KEY_R)) {
			m_RunningFocalDistanceChange = true;
			m_FocalDistanceDirection = !m_FocalDistanceDirection;
			m_FocalDistanceTimer = 0;
		}


		if (m_RunningFocalDistanceChange) {
			const float maxTime = 1.0f;

			m_FocalDistanceTimer += TimeHandler::getDeltaTime();
			if (m_FocalDistanceTimer >= maxTime) {
				m_RunningFocalDistanceChange = false;
				m_FocalDistanceTimer = maxTime;
			}

			float timer = m_FocalDistanceTimer / maxTime;

			if (m_FocalDistanceDirection) {
				timer = 1 - timer;
			}

			float newVal = glm::lerp(5.0f, 16.0f, timer);
			m_DOFTest->setValue("focusDistance", newVal);

		}
	}

	//rotate model
	m_ModelObject->m_Transform.setRotation(glm::vec3(0, TimeHandler::getCurrentTime(), 0));

	//physics
	{
		dynamicsWorld->stepSimulation(TimeHandler::getDeltaTime(), 10);
		{
			btTransform trans;
			trans = fallSphereRigidBody->getWorldTransform();
			bulletToTransform(m_SphereObject[0]->m_Transform, trans);

			trans = groundSphereRigidBody->getWorldTransform();
			bulletToTransform(m_SphereObject[1]->m_Transform, trans);
		}
	}
}

void TestApp::draw() {
	glm::vec3 rot = m_CameraGame->m_Transform.getGlobalRotationEulers();

	Shader::use(m_ShaderBasic);

	ShaderUniformData* uniformPvm = m_ShaderBasic->m_CommonUniforms.m_ProjectionViewMatrix;
	ShaderUniformData* uniformModel = m_ShaderBasic->m_CommonUniforms.m_ModelMatrix;

	uniformPvm->setData(m_CameraGame);
	//uniformModel->setData(&m_Model->m_Transform);

	Shader::applyUniform(uniformPvm);
	//Shader::applyUniform(uniformModel);

	m_RenderList->draw();

	if (m_RenderDOF) {
		Logging::quickGpuDebugGroupPush("Depth of field");
		Logging::quickTimePush("Depth of field");
		m_DOFTest->use(m_FbGameFrame);
		Framebuffer::framebufferBlit(m_DOFTest->getDOFFramebuffer(), m_FbGameFrame);
		Logging::quickTimePop(m_DebugRunningTimersThisFrame);
		Logging::quickGpuDebugGroupPop();
	}
}

void TestApp::drawUi() {
	ShaderUniformData* uniformPVM;
	ShaderUniformData* uniformModel;
	ShaderUniformData* uniformColor;
	Transform model;

	float fontColor[] = { 0.7f,0.7f,0.7f,1.0f };

	Shader::use(m_ShaderText);
	//get uniforms
	uniformColor = m_ShaderText->m_CommonUniforms.m_Color;
	uniformPVM = m_ShaderText->m_CommonUniforms.m_ProjectionViewMatrix;
	uniformModel = m_ShaderText->m_CommonUniforms.m_ModelMatrix;

	//move ui up one line to show the new line
	model.translate(glm::vec3(10, Window::getMainWindow()->getWindowHeight() - 10, 0));

	uniformPVM->setData(&m_CameraMain->getProjectionViewMatrix());

	uniformModel->setData(&model);
	uniformColor->setData(fontColor);

	Shader::applyUniform(uniformPVM);
	Shader::applyUniform(uniformColor);
	Shader::applyUniform(uniformModel);

	m_TestText->draw();
	model.translate(glm::vec3(0, -m_TestText->getLineOffset(), 0));

	std::string quickText;
	//fps
	{
		uniformModel->setData(&model);
		Shader::applyUniform(uniformModel);

		//quick text draw, not as efficient as using a Text object to render
		quickText = "FPS: ";
		quickText += std::to_string(TimeHandler::getFps());
		float offset = m_Font->drawText(quickText.c_str(), 20);
		model.translate(glm::vec3(0, -offset, 0));
	}
	//vert info
	{
		uniformModel->setData(&model);
		Shader::applyUniform(uniformModel);

		//quick text draw, not as efficient as using a Text object to render
		quickText = "Info: Vertices Rendered: ";
		quickText += std::to_string(Logging::getNumVerticesRendered());
		float offset = m_Font->drawText(quickText.c_str(), 20);
		model.translate(glm::vec3(0, -offset, 0));
	}

	//camera info
	{
		uniformModel->setData(&model);
		Shader::applyUniform(uniformModel);

		//quick text draw, not as efficient as using a Text object to render
		quickText = "Camera:---- Pos: (";
		quickText += std::to_string(m_CameraGame->m_Transform.getGlobalPosition().x) + ", ";
		quickText += std::to_string(m_CameraGame->m_Transform.getGlobalPosition().y) + ", ";
		quickText += std::to_string(m_CameraGame->m_Transform.getGlobalPosition().z) + ") ";
		quickText += "Rot (";
		quickText += std::to_string(m_CameraGame->m_Transform.getGlobalRotationEulers().x) + ", ";
		quickText += std::to_string(m_CameraGame->m_Transform.getGlobalRotationEulers().y) + ", ";
		quickText += std::to_string(m_CameraGame->m_Transform.getGlobalRotationEulers().z) + ")";
		float offset = m_Font->drawText(quickText.c_str(), 26);
		model.translate(glm::vec3(0, -offset, 0));
	}

	{
		uniformModel->setData(&model);
		Shader::applyUniform(uniformModel);

		quickText = "DOF NUM_1 " + std::string(m_RenderDOF ? "on" : "Off");
		if (m_RenderDOF) {
			quickText += " (Auto Depth NUM_2 " + std::string(m_AutoDepth ? "On" : "Off") + ")\n";
			if (m_AutoDepth) {
				quickText += " Distance: ";
				quickText += std::to_string(m_CurrentDepth);
				quickText += " Current:";
				quickText += std::to_string(m_DOFTest->getValue("focusDistance"));
				quickText += "\n falloff: ";
				quickText += std::to_string(m_DOFTest->getValue("falloff"));
			} else {
				quickText += " R Change Focus Distance ";
				quickText += std::to_string(m_DOFTest->getValue("focusDistance"));
				quickText += "\n T Change Falloff Distance ";
				quickText += std::to_string(m_DOFTest->getValue("falloff"));
			}
			//quick text draw, not as efficient as using a Text object to render

		}

		float offset = m_Font->drawText(quickText.c_str(), 20);
		model.translate(glm::vec3(0, -offset, 0));
	}
}