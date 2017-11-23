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

#include "Texture.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"

#include "Font.h"
#include "Text.h"

#include "Camera.h"
#include "Framebuffer.h"
#include "Logging.h"

void TestApp::startUp() {

	m_TestText = new Text(m_Font);
	m_TestText->generateText(R"(Side Shooter
		Move: WASDQE)");

	m_Model = new Model();
	m_Model->load("Models/Nanosuit/nanosuit.obj");
}

void TestApp::shutDown() {
	m_Model->unload();
	//m_1x1WhiteTexture->unload();

	delete m_TestText;
}

void TestApp::update() {
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
	Shader::use(m_ShaderBasic);

	ShaderUniformData* uniformPvm = m_ShaderBasic->m_CommonUniforms.m_ProjectionViewMatrix;
	ShaderUniformData* uniformModel = m_ShaderBasic->m_CommonUniforms.m_ModelMatrix;

	Transform model;

	uniformPvm->setData(m_CameraGame);
	uniformModel->setData(&model);

	Shader::applyUniform(uniformPvm);
	Shader::applyUniform(uniformModel);

	m_Model->draw();
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
	model.translate(glm::vec3(0, Window::getMainWindow()->getWindowHeight() + -m_Font->getLineOffset(m_TestText->getNumberOfLines()-1), 0));

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
		float offset = m_Font->drawText(quickText.c_str());
		model.translate(glm::vec3(0, -offset, 0));
	}
	//vert info
	{
		uniformModel->setData(&model);
		Shader::applyUniform(uniformModel);

		//quick text draw, not as efficient as using a Text object to render
		quickText = "Info: Vertices Rendered: ";
		quickText += std::to_string(Logging::getNumVerticesRendered());
		float offset = m_Font->drawText(quickText.c_str());
		model.translate(glm::vec3(0, offset, 0));
	}
}