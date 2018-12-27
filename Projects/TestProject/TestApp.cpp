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

#include "Font.h"
#include "Text.h"

#include "Framebuffer.h"
#include "Texture.h"
#include "Shader.h"
#include "PostEffects/DepthOfField.h"

#include "Object.h"
#include "Camera.h"
#include "CameraFly.h"

#include "Terrain.h"

#include "Multithreading/MtmThread.h"
#include <BulletCollision/CollisionShapes/btStridingMeshInterface.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include "Conversions.h"

#undef min
#undef max

void TestApp::startUp() {

	//--------------------------------------  TEXT TESTING
	m_TestText = new Text(m_Font);
	m_OpenGLThread->queueMethod(this, [](void* tp) {
		//m_TestText->generateText(R"(TEST APP
		//	Move: WASDQE, Arrow keys
		//	Text Size/[Color = 255, 0 ,0]Color [Color=128,64,64,128]Test:[Color = ]
		//	[Size=25]B[Size=32]B Size test![Size=28]![Size=22]![Size=18]![Size=14]!)", 38, glm::vec4(72, 190, 32, 255) / 255.0f);
		((TestApp*)tp)->m_TestText->generateText(R"(TEST APP
		Move: WASDQE, Arrow keys)", 20);
	});

	//--------------------------------------  CAMERA TESTING
	m_CameraGame = new CameraFly();
	m_CameraGame->m_Transform.setPosition(glm::vec3(-5.842, 7.482, 1.879));
	m_CameraGame->m_Transform.setRotation(glm::vec3(40.6868, -74.030, 0));
	((CameraFly*)m_CameraGame)->m_KeyRotationSpeed = 300;
	((CameraFly*)m_CameraGame)->m_KeyboardKeys.MoveUp = KEY_E;
	((CameraFly*)m_CameraGame)->m_KeyboardKeys.MoveDown = KEY_Q;
	//((CameraFly*)m_CameraGame)->m_RotationMode |= CameraFly::RotationModes::MOUSE;


	//--------------------------------------  DOF TESTING
	{
		m_DOFTest = new DepthOfField();
		m_OpenGLThread->queueMethod(this, [](void* tp) {
			((TestApp*)tp)->m_DOFTest->create();
		});
	}

	//--------------------------------------  MODEL LOADING TESTING
	{
		m_Model = new Model();
		m_Ground = new Model();
		m_GrassModel = new Model();
		m_LoadingThread->queueMethod(this, [](void* tp) {
			((TestApp*)tp)->m_Model->load("Models/Nanosuit/nanosuit.obj");
		((TestApp*)tp)->m_Ground->load("Models/test/Ground.obj");
		((TestApp*)tp)->m_GrassModel->load("Models/test/GrassPack/Grass_02.obj");
		});

		//printf("grass gen Start\n");
		m_GrassBatch = new MeshBatch();
		//if (false) {
		//	const unsigned int NumOfGrass = 2000;
		//	const unsigned int sizeOfGround = 50;
		//	for (int i = 0; i < NumOfGrass; i++) {
		//		glm::mat4 position;
		//		glm::vec3 pos;// = m_Ground->m_Meshs[0]->getVertexPosition(i);
		//
		//		pos.x = (((rand() % 10000) / 10000.0f) * (sizeOfGround * 2)) - sizeOfGround;
		//		pos.y = 1;
		//		pos.z = (((rand() % 10000) / 10000.0f) * (sizeOfGround * 2)) - sizeOfGround;
		//
		//		//printf("Pos %i (%f,%f,%f) ---- ", i, pos.x, pos.y, pos.z);
		//		glm::vec3 height = m_Ground->m_Meshs[0]->getClosestPosition(pos);
		//		pos.y = height.y;
		//		//pos = height;
		//		//printf("Pos %i (%f,%f,%f)\n", i, pos.x, pos.y, pos.z);
		//		//const glm::vec3 posOnGround = glm::vec3(i,0,0);
		//		position = glm::translate(glm::mat4(1), pos);
		//		position = glm::rotate(position, ((rand() % 10000) / 10000.0f) * 360, glm::vec3(0, 1, 0));
		//		position = glm::scale(position, glm::vec3(((rand() % 10000) / 10000.0f) * 3));
		//		m_GrassBatch->add(m_GrassModel, position);
		//	}
		//	m_OpenGLThread->queueMethod(this, [](void* tp) {
		//		((TestApp*)tp)->m_GrassBatch->bind();
		//	});
		//}
		//printf("grass gen Finished\n");

		//m_ModelObject = new Object(m_Model);
		//m_GroundObject = new Object(m_Ground);
		//m_GrassBatchObject = new Object(m_GrassBatch);
		m_ModelObject = new Object(nullptr);
		//m_GroundObject = new Object(m_Ground);
		m_GrassBatchObject = new Object(m_GrassBatch->m_CombinedMesh);

		m_Terrain = new Terrain();
		m_Terrain->generate();

		//for (int x = -100; x < 100; x += 10) {
		//	for (int y = -100; y < 100; y += 10) {
		//		Object* newModel = new Object(m_Model);
		//		newModel->m_Transform.setPosition(glm::vec3(x, 0, y));
		//		m_RenderList->addObject(newModel);
		//		m_MoreModelObjects.push_back(newModel);
		//	}
		//
		//}
	}

	//-------------------------------------- SCREENSPACE SELECTION
	{
		m_OpenGLThread->queueMethod(this, [](void* tp) {
			((TestApp*)tp)->m_SelectionShader = new Shader();
			((TestApp*)tp)->m_SelectionShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/basic.vert");
			((TestApp*)tp)->m_SelectionShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/basic.frag");
			((TestApp*)tp)->m_SelectionShader->linkShader();

			((TestApp*)tp)->m_SelectionMesh = new Mesh();
			//((TestApp*)tp)->m_SelectionMesh->createPlane(true);
			((TestApp*)tp)->m_SelectionMesh->createBox();
		});
	}


	//-------------------------------------- PHYSICS TESTING
	{
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


		m_LoadingThread->waitForThread();
		//{
		//	//memory leak's everywhere!
		//	btTriangleMesh* mesh = new btTriangleMesh();
		//	mesh->preallocateIndices(m_Ground->m_Meshs[0]->m_Indices.size());
		//	mesh->preallocateVertices(m_Ground->m_Meshs[0]->m_Vertices.size());
		//	for (unsigned int i = 0; i < m_Ground->m_Meshs[0]->m_Indices.size(); i += 3) {
		//		glm::vec3 pos0 = m_Ground->m_Meshs[0]->m_Vertices[m_Ground->m_Meshs[0]->m_Indices[i + 0]].position;
		//		glm::vec3 pos1 = m_Ground->m_Meshs[0]->m_Vertices[m_Ground->m_Meshs[0]->m_Indices[i + 1]].position;
		//		glm::vec3 pos2 = m_Ground->m_Meshs[0]->m_Vertices[m_Ground->m_Meshs[0]->m_Indices[i + 2]].position;
		//		mesh->addTriangle(
		//			btVector3(pos0.x, pos0.y, pos0.z),
		//			btVector3(pos1.x, pos1.y, pos1.z),
		//			btVector3(pos2.x, pos2.y, pos2.z));
		//	}
		//
		//	//btCollisionShape* groundMesh = new btTriangleMeshShape(smi);
		//	btCollisionShape* groundMesh = new btBvhTriangleMeshShape(mesh, true);
		//
		//	//btCollisionShape* groundMesh = new btConvexHullShape(points[0].m_floats, points.size(),16);
		//	btDefaultMotionState* groundMeshMS = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
		//	btRigidBody::btRigidBodyConstructionInfo groundMeshRBCI(0, groundMeshMS, groundMesh, btVector3(0, 0, 0));
		//	groundMeshRBCI.m_restitution = 0.75f;
		//	btRigidBody* groundMeshRB = new btRigidBody(groundMeshRBCI);
		//
		//	dynamicsWorld->addRigidBody(groundMeshRB);
		//}

	}
	return;
	printf("grass gen Start\n");
	{
		if (m_GrassBatch == nullptr) {
			m_GrassBatch = new MeshBatch();
		}
		const unsigned int NumOfGrass = 5000;
		const unsigned int sizeOfGround = 50;
		unsigned int numSpawned = 0;
		unsigned int percentage = 0;
		for (int i = 0; i < NumOfGrass; i++) {
			glm::mat4 position;
			glm::vec3 pos;// = m_Ground->m_Meshs[0]->getVertexPosition(i);

			pos.x = (((rand() % 10000) / 10000.0f) * (sizeOfGround * 2)) - sizeOfGround;
			pos.y = 1;
			pos.z = (((rand() % 10000) / 10000.0f) * (sizeOfGround * 2)) - sizeOfGround;

			btCollisionWorld::ClosestRayResultCallback raycastCallback(btVector3(pos.x, 100, pos.z), btVector3(pos.x, -100, pos.z));
			dynamicsWorld->rayTest(btVector3(pos.x, 100, pos.z), btVector3(pos.x, -100, pos.z), raycastCallback);
			if (!raycastCallback.hasHit()) {
				continue;
			}
			pos.y = raycastCallback.m_hitPointWorld.getY();

			position = glm::translate(glm::mat4(1), pos);
			position = glm::rotate(position, ((rand() % 10000) / 10000.0f) * 360, glm::vec3(0, 1, 0));
			position = glm::scale(position, glm::vec3(((rand() % 10000) / 10000.0f) * 3) + 0.25f);
			m_GrassBatch->add(m_GrassModel, position);
			numSpawned++;
			if (i%(NumOfGrass / 10) == 0) {
				printf("grass gen %i percent compleate\n", (++percentage) * 10);
			}
		}
		m_OpenGLThread->queueMethod(this, [](void* tp) {
			((TestApp*)tp)->m_GrassBatch->bind();
		});
	printf("grass gen Finished SPAWNED %i/%i\n", numSpawned,NumOfGrass);
	}

}

void TestApp::shutDown() {
	m_Model->unload();
	m_Ground->unload();
	m_GrassModel->unload();

	delete m_SphereObject[0];
	delete m_SphereObject[1];

	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfig;
	delete broadphase;


	delete fallSphereMotionState;
	delete fallSphereRigidBody;
	delete fallSphere;

	delete groundPlaneMotionState;
	delete groundSphereRigidBody;
	delete groundPlane;

	delete m_GrassBatch;
	delete m_GrassBatchObject;

	delete m_ModelObject;
	delete m_GroundObject;
	delete m_DOFTest;

	delete m_TestText;

	delete m_SelectionShader;
	delete m_SelectionMesh;

	delete m_Terrain;

	for (int i = 0; i < m_MoreModelObjects.size(); i++) {
		delete m_MoreModelObjects[i];
	}
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
		static const float values[] = { 2.0f, 20.0f, 0, 100.0f };
		m_DOFTest->setValue("falloff", values[(val++) % 4]);

	}

	if (Input::wasKeyPressed(KEY_KP_2)) {
		m_AutoDepth ^= 1;
		m_RunningFocalDistanceChange = true;
	}

	if (m_AutoDepth) {
		//run 3 times a second, because it's slow
		if (TimeHandler::getCurrentFrameNumber() % 20 == 0) {
			printf("Auto DOF: updating distance\n");
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
	//if(dynamicsWorld){
	//	dynamicsWorld->stepSimulation(TimeHandler::getDeltaTime(), 10);
	//	{
	//		btTransform trans;
	//		trans = fallSphereRigidBody->getWorldTransform();
	//		bulletToTransform(m_SphereObject[0]->m_Transform, trans);
	//
	//		trans = groundSphereRigidBody->getWorldTransform();
	//		bulletToTransform(m_SphereObject[1]->m_Transform, trans);
	//	}
	//}

	if (Input::isMouseDown(0)) {
		if (Input::wasMousePressed(0)) {
			m_SelectionFirstPoint = Input::getMousePos();
		} else {
			m_SelectionSecondPoint = Input::getMousePos();
			m_ShowSelection = glm::distance(m_SelectionFirstPoint, m_SelectionSecondPoint) > 50;

			if (m_ShowSelection) {
				m_InSelection.clear();
				for (int i = 0; i < 2; i++) {
					glm::vec2 pos = m_SphereObject[i]->m_Transform.ToScreenSpace(m_CameraGame);
					glm::vec2 windowSize = glm::vec2(Window::getMainWindow()->getFramebufferWidth(), Window::getMainWindow()->getFramebufferHeight());
					glm::vec2 min = glm::min(m_SelectionFirstPoint, m_SelectionSecondPoint) / windowSize;
					glm::vec2 max = glm::max(m_SelectionFirstPoint, m_SelectionSecondPoint) / windowSize;
					if (min.x < pos.x && min.y < pos.y) {
						if (max.x > pos.x && max.y > pos.y) {
							m_InSelection.push_back(m_SphereObject[i]);
						}
					}
				}
			}

		}
	} else {
		m_InSelection.clear();
		m_ShowSelection = false;
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

	//m_ModelObject->m_Renderable->draw();
	//m_GrassBatchObject->m_Renderable->draw();
	//m_SphereObject[0]->m_Renderable->draw();
	//m_SphereObject[1]->m_Renderable->draw();
	m_Terrain->draw();

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

	float fontColor[] = { 0.7f, 0.7f, 0.7f, 1.0f };

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

		if (m_ShowSelection) {

			//selection info
			{
				uniformModel->setData(&model);
				Shader::applyUniform(uniformModel);

				//quick text draw, not as efficient as using a Text object to render
				quickText = "Selected: ";
				quickText += std::to_string(m_InSelection.size());
				for (int i = 0; i < m_InSelection.size(); i++) {
					quickText += "\n" + m_InSelection[i]->m_Transform.m_Name;
				}
				float offset = m_Font->drawText(quickText.c_str(), 20);
				model.translate(glm::vec3(0, -offset, 0));
			}

			Shader::use(m_SelectionShader);
			uniformPVM = m_SelectionShader->m_CommonUniforms.m_ProjectionViewMatrix;
			uniformModel = m_SelectionShader->m_CommonUniforms.m_ModelMatrix;

			uniformPVM->setData(&m_CameraMain->getProjectionViewMatrix());
			m_SelectionShader->m_CommonUniforms.m_Color->setData(1.0f, 0.0f, 1.0f, 1.0f);

			glm::vec2 pos = (m_SelectionFirstPoint + m_SelectionSecondPoint) / 2;
			glm::vec2 size = glm::abs(m_SelectionFirstPoint - m_SelectionSecondPoint) / 2;

			//model.setPosition(glm::vec3(glm::sin(TimeHandler::getCurrentTime() / 4) * 200, glm::sin(TimeHandler::getCurrentTime() / 2) * 200, glm::sin(TimeHandler::getCurrentTime()) * 200));
			model.setPosition(glm::vec3(pos.x, Window::getMainWindow()->getFramebufferHeight() - pos.y, 0));
			//model.setPosition(glm::vec3(100,-100,0));
			float ratio = Window::getMainWindow()->getFramebufferWidth() / Window::getMainWindow()->getFramebufferHeight();
			model.setScale(glm::vec3(size.x, size.y / ratio, 1));
			model.setRotation(glm::vec3(0, 0, 0));

			uniformModel->setData(&model);
			m_SelectionShader->checkUniformChanges();

			m_SelectionMesh->draw();
		}
	}



}