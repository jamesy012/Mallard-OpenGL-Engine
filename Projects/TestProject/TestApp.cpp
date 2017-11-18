#include "TestApp.h"

#include <stdio.h>

#include <gl\glew.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "TimeHandler.h"
#include "Window.h"
#include "Transform.h"
#include "Input.h"

#include "Texture.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"

#include "Font.h"
#include "Text.h"

#include "Camera.h"
#include "Framebuffer.h"

void TestApp::startUp() {
	m_Texture = new Texture();

	//m_Texture->load("Textures/imageTest.jpg");
	//m_Texture->load("Textures/imageTest.png");
	m_Texture->load("Textures/redWhite.jpg");

	m_Model = new Model();

	m_Model->load("Models/Nanosuit/nanosuit.obj");
	//m_Model->load("Models/ModelTest/ModelTest.fbx");
	//m_Model->load("Models/ModelTest/ModelTest.obj");


	m_Shader = new Shader();

	m_Shader->createSimpleShader(true);

	m_Mesh = new Mesh();
	m_Mesh->createPlane(true);
	m_Mesh->setTexture(m_Texture);

	m_Font = new Font();
	m_Font->loadFont("c:/windows/fonts/comic.ttf", 48);

	m_TestText = new Text(m_Font);
	m_TestText->generateText("~ Test Text, 1234!@#$ ~\nNew Line");

	m_IntensityText = new Text(m_Font);
	m_IntensityText->generateText("Blur");


	//todo, move to Font
	m_TextShader = new Shader();
	Font::generateShaderCode(m_TextShader);
	m_TextShader->linkShader();


	//set up camera
	m_GameCamera->m_Transform.setPosition(glm::vec3(0, 0, 20));

	//FRAME BUFFER TEST
	m_FbTest = new Framebuffer();
	m_FbTest->setSize(512, 512);
	m_FbTest->createRenderTarget();

	m_FbTestBlured = new Framebuffer();
	m_FbTestBlured->setSize(512, 512);
	m_FbTestBlured->createRenderTarget();//can do without the depth buffer
	//m_FbTest->genFramebuffer();

	m_FbPlane = new Mesh();
	m_FbPlane->createPlane(false);
	m_FbPlane->setTexture(m_FbTest->getTexture());

	m_FbCamera = new Camera();
	//m_FbCamera->setPerspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	m_FbCamera->setOrthographic(-16, 16, -16, 16, 0.1f, 1000.0f);
	m_FbCamera->m_Transform.setPosition(glm::vec3(10, 10, 10));
	m_FbCamera->m_Transform.setLookAt(glm::vec3(0, 0, 0));

	m_PostprocessingBlur = new Shader();
	m_PostprocessingBlur->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
	m_PostprocessingBlur->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/BoxBlur.frag");
	m_PostprocessingBlur->linkShader();

}

void TestApp::shutDown() {
	m_Model->unload();
	m_Texture->unload();
	delete m_Shader;
	delete m_Mesh;
	delete m_Font;
	delete m_TestText;
	delete m_TextShader;

	delete m_FbCamera;
	delete m_FbTest;
	delete m_FbTestBlured;
	delete m_FbPlane;

	delete m_IntensityText;
	delete m_PostprocessingBlur;
}

void TestApp::update() {
	//printf("Window: (%i,%i)",m_AppWindow->getWindowWidth(),m_AppWindow->getWindowHeight());
	//
	//// Keep running
	//printf("Frame: %i dt: %f\n", TimeHandler::getCurrentFrameNumber(), TimeHandler::getDeltaTime());

	if (Input::wasKeyPressed(73)) {
		m_PostprocessingBlur->reloadShaders();
	}
	int leftOrRight = Input::isKeyDown(263) << 0 | Input::isKeyDown(262) << 1;
	//printf("left or right: %i\n", leftOrRight);
	if (leftOrRight != 0) {
		ShaderUniformData* blurIntensity = m_PostprocessingBlur->getUniform(ShaderUniformTypes::FLOAT, "Intensity");
		float value = *(float*)blurIntensity->getDataVoid();
		printf("left or right: Before: %f", value);
		if (leftOrRight & (1)) {//left
			value -= TimeHandler::getDeltaTime() * 5;
			if (value < 0) {
				value = 0;
			}
		}
		if (leftOrRight & (2)) {//right
			value += TimeHandler::getDeltaTime() * 5;
		}
		m_IntensityText->generateText("Blur: " + std::to_string(value));
		printf(" After: %f\n", value);
		blurIntensity->setData(&value);
		Shader::use(m_PostprocessingBlur);
		m_PostprocessingBlur->applyUniform(blurIntensity);
	}
}

void TestApp::draw() {
	//framebuffer test
	runFramebufferTest();

	Shader::use(m_Shader);

	Transform model;

	ShaderUniformData* uniformPVM;
	ShaderUniformData* uniformModel;

	///NORMAL RENDER

	//set up data
	static float lerpTime = 0.6f;
	static float lerpScale = 0.25f;
	lerpTime += lerpScale * TimeHandler::getDeltaTime();
	if (lerpTime > 1 || lerpTime < 0) {
		lerpScale = -lerpScale;
	}
	glm::vec3 camPos = glm::lerp(glm::vec3(-5, 2, 20), glm::vec3(5, -2, 20), lerpTime);
	m_GameCamera->m_Transform.setPosition(camPos);

	model.rotate(glm::vec3(TimeHandler::getCurrentTime()*0.32f, 0, 0) * 100.0f);

	//get uniforms
	uniformPVM = m_Shader->m_CommonUniforms.m_ProjectionViewMatrix;
	uniformModel = m_Shader->m_CommonUniforms.m_ModelMatrix;

	//set data
	uniformPVM->setData(&m_MainCamera->getProjectionViewMatrix());
	uniformModel->setData(&model);

	//apply uniforms
	Shader::applyUniform(uniformPVM);
	Shader::applyUniform(uniformModel);

	//draw models
	m_Model->draw();
	m_Mesh->draw();

	//model.setPosition(glm::vec3(0, 0, -5) + (m_GameCamera->m_Transform.getLocalPosition() * glm::vec3(1, 1, 0)));
	//model.setScale(glm::vec3(5, 5, 5));
	model.setPosition(glm::vec3(0, 0, -100));
	model.setScale(50.0f);
	model.setRotation(glm::vec3(-90, 0, 0));
	uniformModel->setData(&model);
	Shader::applyUniform(uniformModel);

	m_FbPlane->setTexture(m_FbTestBlured->getTexture());
	m_FbPlane->draw();
}

void TestApp::drawUi() {
	ShaderUniformData* uniformPVM;
	ShaderUniformData* uniformModel;
	ShaderUniformData* uniformColor;
	Transform model;

	Shader::use(m_TextShader);
	//get uniforms
	uniformColor = m_TextShader->m_CommonUniforms.m_Color;
	uniformPVM = m_TextShader->m_CommonUniforms.m_ProjectionViewMatrix;
	uniformModel = m_TextShader->m_CommonUniforms.m_ModelMatrix;

	//move ui up one line to show the new line
	model.setPosition(glm::vec3(0, m_Font->getLineOffset(1), 0));

	uniformPVM->setData(&m_MainCamera->getProjectionViewMatrix());
	//change just the alpha of the uniform
	uniformColor->modifyData(3, (sin(TimeHandler::getCurrentTime())*0.5f + 0.5f));
	uniformModel->setData(&model);

	Shader::applyUniform(uniformPVM);
	Shader::applyUniform(uniformColor);
	Shader::applyUniform(uniformModel);

	//glDisable(GL_CULL_FACE);
	//glDepthMask(GL_TRUE);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_TestText->draw();

	//move the position up 1 more line for the quick draw
	model.translate(glm::vec3(0, m_Font->getLineOffset(1), 0));

	uniformModel->setData(&model);
	Shader::applyUniform(uniformModel);

	//update text color to a redish color
	float newFontColor[] = { 0.9f,0.4f,0.5f,1.0f };
	uniformColor->setData(newFontColor);
	Shader::applyUniform(uniformColor);
	//testing the quick and not as efficient text rendering
	std::string quickText = "FPS: ";
	quickText += std::to_string(TimeHandler::getFps());
	m_Font->drawText(quickText.c_str());

	//reset font color after drawing with it, do not apply because we dont need to
	//next time it renders the reset values will be applied by our alpha change
	float resetFontColor[] = { 1.0f,1.0f,1.0f,1.0f };
	uniformColor->setData(resetFontColor);

	//draw Blur text
	model.setPosition(glm::vec3(Window::getMainWindow()->getWindowWidth() / 2.0f, Window::getMainWindow()->getWindowHeight() / 4.0f, 0));
	uniformModel->setData(&model);
	Shader::applyUniform(uniformModel);
	m_IntensityText->draw();

}

void TestApp::runFramebufferTest() {
	/*
	Shader::use(m_Shader);
	Framebuffer::use(m_FbTest);

	Transform model;

	ShaderUniformData* uniformPVM;
	ShaderUniformData* uniformModel;

	///NORMAL RENDER
	model.rotate(glm::vec3(TimeHandler::getCurrentTime()*0.32f, TimeHandler::getCurrentTime()*0.17f, 0) * 100.0f);

	//get uniforms
	uniformPVM = m_Shader->m_CommonUniforms.m_ProjectionViewMatrix;
	uniformModel = m_Shader->m_CommonUniforms.m_ModelMatrix;

	//set data
	uniformPVM->setData(&m_FbCamera->getProjectionViewMatrix());
	uniformModel->setData(&model);

	//apply uniforms
	Shader::applyUniform(uniformPVM);
	Shader::applyUniform(uniformModel);

	//draw models
	m_Model->draw();
	m_Mesh->draw();
	*/
	//render the framebuffer to a new one which only has the blur data
	Framebuffer::use(m_FbTestBlured);
	Shader::use(m_PostprocessingBlur);
	//set it to the other framebuffers texture
	m_FbPlane->setTexture(m_FbGameFrameCopy->getTexture());
	m_FbPlane->draw();

	Framebuffer::use(nullptr);
}
