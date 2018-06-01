#include "TestApp.h"

#include <stdio.h>

#include <gl\glew.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "TimeHandler.h"
#include "Window.h"
#include "Transform.h"
#include "Input.h"
#include "Keys.h"

#include "Model.h"
#include "Mesh.h"
#include "Shader.h"

#include "Font.h"
#include "Text.h"

#include "Camera.h"
#include "Logging.h"

#include "Framebuffer.h"


void TestApp::startUp() {

	m_TestText = new Text(m_Font);
	m_TestText->generateText(R"(TEST APP
		Move: WASDQE, Arrow keys
		Text Size/[Color = 255, 0 ,0]Color [Color=128,64,64,128]Test:[Color = ]
		[Size=25]B[Size=32]B Size test![Size=28]![Size=22]![Size=18]![Size=14]!)", 38, glm::vec4(72,190,32,255)/255.0f);


	m_CameraGame->m_Transform.setPosition(glm::vec3(-5.842, 7.482, 1.879));
	m_CameraGame->m_Transform.setRotation(glm::vec3(40.6868, -74.030, 0));

	m_Model = new Model();
	m_Model->load("Models/Nanosuit/nanosuit.obj");

	m_QuadMesh = new Mesh();
	m_QuadMesh->createPlane(false);


	m_QuadMesh->m_Transform.setParent(&m_CameraGame->m_Transform);
	m_QuadMesh->m_Transform.translate(glm::vec3(-5, 0, -20), false);
	m_QuadMesh->m_Transform.rotate(glm::vec3(90, 0, 0));
	m_QuadMesh->m_Transform.setScale(5);

	m_DOFTest.create();


	m_RenderList.addObject(m_Model);
}

void TestApp::shutDown() {
	m_Model->unload();

	delete m_QuadMesh;
	delete m_TestText;
}

void TestApp::update() {
	if (Input::wasKeyPressed(KEY_F1)) {
		m_Flags.m_RunDebugTimers ^= 1;
	}

	if (Input::wasKeyPressed(KEY_KP_1)) {
		m_RenderDOF ^= 1;
	}

	if (Input::wasKeyPressed(KEY_R)) {
		m_RunningFocalDistanceChange = true;
		m_FocalDistanceDirection = !m_FocalDistanceDirection;
		m_FocalDistanceTimer = 0;
	}

	if (Input::wasKeyPressed(KEY_T)) {
		static bool val = true;
		m_DOFTest.setValue("falloff", (val = !val)?2.0f:20.0f);

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

		float newVal = glm::lerp(5.0f, 16.0f,timer);
		m_DOFTest.setValue("focusDistance", newVal);

	}

	float cameraMoveSpeed = 10.0f;
	float cameraRotateRpeed = 40.0f;
	if (Input::isKeyDown(KEY_E)) {
		m_CameraGame->m_Transform.translate(glm::vec3(0, cameraMoveSpeed, 0)*TimeHandler::getDeltaTime(), false);
	}
	if (Input::isKeyDown(KEY_Q)) {
		m_CameraGame->m_Transform.translate(glm::vec3(0, -cameraMoveSpeed, 0)*TimeHandler::getDeltaTime(), false);
	}
	if (Input::isKeyDown(KEY_S)) {
		m_CameraGame->m_Transform.translate(glm::vec3(0, 0, cameraMoveSpeed)*TimeHandler::getDeltaTime(), false);
	}
	if (Input::isKeyDown(KEY_W)) {
		m_CameraGame->m_Transform.translate(glm::vec3(0, 0, -cameraMoveSpeed)*TimeHandler::getDeltaTime(), false);
	}
	if (Input::isKeyDown(KEY_D)) {
		m_CameraGame->m_Transform.translate(glm::vec3(cameraMoveSpeed, 0, 0)*TimeHandler::getDeltaTime(), false);
	}
	if (Input::isKeyDown(KEY_A)) {
		m_CameraGame->m_Transform.translate(glm::vec3(-cameraMoveSpeed, 0, 0)*TimeHandler::getDeltaTime(), false);
	}
	if (Input::isKeyDown(KEY_LEFT)) {
		m_CameraGame->m_Transform.rotate(glm::vec3(0, cameraRotateRpeed, 0)*TimeHandler::getDeltaTime());
	}
	if (Input::isKeyDown(KEY_RIGHT)) {
		m_CameraGame->m_Transform.rotate(glm::vec3(0, -cameraRotateRpeed, 0)*TimeHandler::getDeltaTime());
	}
	if (Input::isKeyDown(KEY_UP)) {
		m_CameraGame->m_Transform.rotate(glm::vec3(cameraRotateRpeed, 0, 0)*TimeHandler::getDeltaTime());
	}
	if (Input::isKeyDown(KEY_DOWN)) {
		m_CameraGame->m_Transform.rotate(glm::vec3(-cameraRotateRpeed, 0, 0)*TimeHandler::getDeltaTime());
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

	m_RenderList.draw();

	if(m_RenderDOF) {
		Logging::quickGpuDebugGroupPush("Depth of field");
		Logging::quickTimePush("Depth of field");
		m_DOFTest.use(m_FbGameFrame);
		Framebuffer::framebufferBlit(m_DOFTest.getDOFFramebuffer(), m_FbGameFrame);
		Logging::quickTimePop(m_DebugRunningTimersThisFrame);
		Logging::quickGpuDebugGroupPop();
	}

	uniformModel->setData(&m_QuadMesh->m_Transform);
	Shader::applyUniform(uniformModel);


	//m_QuadMesh->draw();
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
	model.translate(glm::vec3(0, Window::getMainWindow()->getWindowHeight(), 0));

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
		float offset = m_Font->drawText(quickText.c_str(),20);
		model.translate(glm::vec3(0, -offset, 0));
	}
	//vert info
	{
		uniformModel->setData(&model);
		Shader::applyUniform(uniformModel);

		//quick text draw, not as efficient as using a Text object to render
		quickText = "Info: Vertices Rendered: ";
		quickText += std::to_string(Logging::getNumVerticesRendered());
		float offset = m_Font->drawText(quickText.c_str(),20);
		model.translate(glm::vec3(0, offset, 0));
	}
}