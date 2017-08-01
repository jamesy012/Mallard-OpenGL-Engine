#include "TestApp.h"

#include <stdio.h>

#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "TimeHandler.h"
#include "Window.h"
#include "Transform.h"

#include "Texture.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"

#include "Font.h"
#include "Text.h"

void TestApp::startUp() {
	//Texture* t1 = new Texture();
	//Texture* t2 = new Texture();
	//Texture* t3 = new Texture();
	//Texture* t4 = new Texture();
	m_Texture = new Texture();

	//m_Texture->load("Textures/imageTest.jpg");
	//m_Texture->load("Textures/imageTest.png");
	m_Texture->load("Textures/redWhite.jpg");
	//m_Texture->load1x1Texture();
	//t1->load("Textures/imageTest.png");
	//t2->load("test2");
	//t3->load("Textures/imageTest.png");
	//t4->load("Textures/imageTest.png");
	//
	//t1->unload();
	//t2->unload();
	//t3->unload();
	//t4->unload();

	m_Model = new Model();

	//m_Model->load("Models/Nanosuit/nanosuit.obj");
	//m_Model->load("Models/ModelTest/ModelTest.fbx");
	m_Model->load("Models/ModelTest/ModelTest.obj");


	m_Shader = new Shader();

	m_Shader->createSimpleShader(true);

	m_Mesh = new Mesh();
	m_Mesh->createPlane();
	m_Mesh->setTexture(m_Texture);

	//delete t1;
	//delete t2;
	//delete t3;
	//delete t4;

	m_Font = new Font();
	m_Font->loadFont("c:/windows/fonts/comic.ttf", 48);

	m_TestText = new Text(m_Font);
	m_TestText->generateText("~ Test Text, 1234!@#$ ~\nNew Line");

	//todo, move to Font
	

	m_TextShader = new Shader();
	Font::generateShaderCode(m_TextShader);
	m_TextShader->linkShader();


	//set up camera
	m_GameCamera->m_Transform.setPosition(glm::vec3(0, 0, 20));

}

void TestApp::shutDown() {
	m_Model->unload();
	m_Texture->unload();
	delete m_Shader;
	delete m_Mesh;
	delete m_Font;
	delete m_TestText;
	delete m_TextShader;
}

void TestApp::update() {
	//printf("Window: (%i,%i)",m_AppWindow->getWindowWidth(),m_AppWindow->getWindowHeight());
	//
	//// Keep running
	//printf("Frame: %i dt: %f\n", TimeHandler::getCurrentFrameNumber(), TimeHandler::getDeltaTime());
}

void TestApp::draw() {
	Transform model;

	ShaderUniformData* uniformPVM;
	ShaderUniformData* uniformModel;

	///NORMAL RENDER
	m_Shader->use();

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
}

void TestApp::drawUi() {
	ShaderUniformData* uniformPVM;
	ShaderUniformData* uniformModel;
	ShaderUniformData* uniformColor;
	Transform model;

	m_TextShader->use();
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
	m_Font->drawText("    Font Text Draw Test");

	//reset font color after drawing with it, do not apply because we dont need to
	//next time it renders the reset values will be applied by our alpha change
	float resetFontColor[] = { 1.0f,1.0f,1.0f,1.0f };
	uniformColor->setData(resetFontColor);


}
