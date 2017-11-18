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

void TestApp::startUp() {
	m_Model = new Model();
	m_Model->load("Models/Nanosuit/nanosuit.obj");
	//m_Model->load("Models/ModelTest/ModelTest.fbx");
	//m_Model->load("Models/ModelTest/ModelTest.obj");

	m_Font = new Font();
	m_Font->loadFont("c:/windows/fonts/comic.ttf", 48);

	m_TestText = new Text(m_Font);
	m_TestText->generateText("Shadow Test\nMode: WASDQE, Rotate: Arrow keys, Other: ZXCV");

	//todo, move to Font
	m_TextShader = new Shader();
	Font::generateShaderCode(m_TextShader);
	m_TextShader->linkShader();

	//set up camera
	m_GameCamera->m_Transform.setPosition(glm::vec3(0, 5, 20));

	m_1x1WhiteTexture = new Texture();
	m_1x1WhiteTexture->load1x1Texture();

	m_GroundPlane = new Mesh();
	m_GroundPlane->createPlane(false);
	m_GroundPlane->setTexture(m_1x1WhiteTexture);

	m_ShadowGenerationShader = new Shader();
	m_ShadowGenerationShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders\\Shadows\\GenShadow.vert");
	m_ShadowGenerationShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders\\Shadows\\GenShadow.frag");
	m_ShadowGenerationShader->linkShader();

	m_ShadowDrawShader = new Shader();
	m_ShadowDrawShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders\\Shadows\\DrawShadow.vert");
	m_ShadowDrawShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders\\Shadows\\DrawShadow.frag");
	m_ShadowDrawShader->linkShader();

	m_DirectionalLightFb = new Framebuffer();
	m_DirectionalLightFb->setSize(1024, 1024);
	m_DirectionalLightFb->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::DEPTH);
	m_DirectionalLightFb->genFramebuffer();

	m_LightDir = glm::normalize(glm::vec3(1.0f, 2.5f, 1.0f));

	glm::mat4 lightProjection = glm::ortho<float>(-40, 40, -40, 40, -100, 100);
	glm::mat4 lightView = glm::lookAt(m_LightDir, glm::vec3(0), glm::vec3(0, 1, 0));

	m_LightMatrix = lightProjection * lightView;
}

void TestApp::shutDown() {
	m_Model->unload();
	m_1x1WhiteTexture->unload();

	delete m_Font;
	delete m_TestText;
	delete m_TextShader;

	delete m_GroundPlane;

	delete m_ShadowGenerationShader;
	delete m_ShadowDrawShader;

	delete m_DirectionalLightFb;
}

void TestApp::update() {
	float cameraMoveSpeed = 10.0f;
	float cameraRotateRpeed = 40.0f;
	if (Input::isKeyDown(KEY_E)) {
		m_GameCamera->m_Transform.translate(glm::vec3(0, cameraMoveSpeed, 0)*TimeHandler::getDeltaTime(),false);
	}
	if (Input::isKeyDown(KEY_Q)) {
		m_GameCamera->m_Transform.translate(glm::vec3(0, -cameraMoveSpeed, 0)*TimeHandler::getDeltaTime(), false);
	}
	if (Input::isKeyDown(KEY_S)) {
		m_GameCamera->m_Transform.translate(glm::vec3(0, 0, cameraMoveSpeed)*TimeHandler::getDeltaTime(), false);
	}
	if (Input::isKeyDown(KEY_W)) {
		m_GameCamera->m_Transform.translate(glm::vec3(0, 0, -cameraMoveSpeed)*TimeHandler::getDeltaTime(), false);
	}
	if (Input::isKeyDown(KEY_D)) {
		m_GameCamera->m_Transform.translate(glm::vec3(cameraMoveSpeed, 0, 0)*TimeHandler::getDeltaTime(), false);
	}
	if (Input::isKeyDown(KEY_A)) {
		m_GameCamera->m_Transform.translate(glm::vec3(-cameraMoveSpeed, 0, 0)*TimeHandler::getDeltaTime(), false);
	}
	if (Input::isKeyDown(KEY_LEFT)) {
		m_GameCamera->m_Transform.rotate(glm::vec3(0, cameraRotateRpeed, 0)*TimeHandler::getDeltaTime());
	}
	if (Input::isKeyDown(KEY_RIGHT)) {
		m_GameCamera->m_Transform.rotate(glm::vec3(0, -cameraRotateRpeed, 0)*TimeHandler::getDeltaTime());
	}
	if (Input::isKeyDown(KEY_UP)) {
		m_GameCamera->m_Transform.rotate(glm::vec3(cameraRotateRpeed, 0, 0)*TimeHandler::getDeltaTime());
	}
	if (Input::isKeyDown(KEY_DOWN)) {
		m_GameCamera->m_Transform.rotate(glm::vec3(-cameraRotateRpeed, 0, 0)*TimeHandler::getDeltaTime());
	}

	if (Input::wasKeyPressed(KEY_Z)) {
		m_AmountPerSide--;
		if (m_AmountPerSide < 0) {
			m_AmountPerSide = 0;
		}
	}
	if (Input::wasKeyPressed(KEY_X)) {
		m_AmountPerSide++;
	}

	if (Input::isKeyDown(KEY_C)) {
		m_Spacing -= 5 * TimeHandler::getDeltaTime();
		if (m_Spacing < 2) {
			m_Spacing = 2;
		}
	}
	if (Input::isKeyDown(KEY_V)) {
		m_Spacing += 5 * TimeHandler::getDeltaTime();
	}



	if (Input::wasKeyPressed(KEY_O)) {
		m_ShadowGenerationShader->reloadShaders();
	}
	if (Input::wasKeyPressed(KEY_P)) {
		m_ShadowDrawShader->reloadShaders();
	}
}

void TestApp::draw() {
	Transform model;
	{
		Framebuffer::use(m_DirectionalLightFb);
		Shader::use(m_ShadowGenerationShader);

		ShaderUniformData* lightMatrix = m_ShadowGenerationShader->m_CommonUniforms.m_ProjectionViewMatrix;

		lightMatrix->setData(&m_LightMatrix);

		Shader::applyUniform(lightMatrix);

		drawModels();
	}
	{
		Framebuffer::use(nullptr);
		Shader::use(m_ShadowDrawShader);

		ShaderUniformData* cameraPvm = m_ShadowDrawShader->m_CommonUniforms.m_ProjectionViewMatrix;
		ShaderUniformData* modelMatrix = m_ShadowDrawShader->m_CommonUniforms.m_ModelMatrix;
		ShaderUniformData* modelNormalRotMatrix = m_ShadowDrawShader->m_CommonUniforms.m_NormalRotMatrix;
		ShaderUniformData* lightMatrix = m_ShadowDrawShader->getUniform(ShaderUniformTypes::MAT4,"lightMatrix");
		ShaderUniformData* lightDir = m_ShadowDrawShader->getUniform(ShaderUniformTypes::VEC3, "lightDir");
		ShaderUniformData* shadowMapTex = m_ShadowDrawShader->getUniform(ShaderUniformTypes::SAMPLER2D, "shadowMap");

		glm::mat4 textureSpaceOffset = glm::mat4(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f
		);

		glm::mat4 offsetLightMatrix = textureSpaceOffset * m_LightMatrix;

		if (cameraPvm != nullptr) {
			cameraPvm->setData(&m_GameCamera->getProjectionViewMatrix());
			Shader::applyUniform(cameraPvm);
		}
		if (lightDir != nullptr) {
			lightDir->setData(glm::value_ptr(m_LightDir));
			Shader::applyUniform(lightDir);
		}
		if (lightMatrix != nullptr) {
			lightMatrix->setData(&offsetLightMatrix);
			Shader::applyUniform(lightMatrix);
		}

		if (shadowMapTex != nullptr) {
			m_DirectionalLightFb->getTexture()->bindAndApplyTexture(3, shadowMapTex);
		}

		drawModels();

		model.setScale(glm::vec3(100, 1, 100));
		model.setRotation(glm::vec3(0, 0, 0));
		glm::mat4 modelMat = model.getGlobalMatrix();
		glm::mat4 normalMat = glm::transpose(glm::inverse(modelMat));
		if (modelMatrix != nullptr) {
			modelMatrix->setData(&modelMat);
			Shader::applyUniform(modelMatrix);
		}
		if (modelNormalRotMatrix != nullptr) {
			modelNormalRotMatrix->setData(&normalMat);
			Shader::applyUniform(modelNormalRotMatrix);
		}

		m_GroundPlane->draw();
	}
	Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, false);
	{
		Shader::use(m_BasicShader);

		ShaderUniformData* cameraPvm = m_BasicShader->m_CommonUniforms.m_ProjectionViewMatrix;
		ShaderUniformData* modelMatrix = m_BasicShader->m_CommonUniforms.m_ModelMatrix;

		model = m_MainCamera->m_Transform;
		model.translate(glm::vec3(-15, 0, -20),false);
		model.rotate(glm::vec3(-90, 0, 0));
		model.setScale(5);

		cameraPvm->setData(&m_GameCamera->getProjectionViewMatrix());
		Shader::applyUniform(cameraPvm);
		modelMatrix->setData(&model);
		Shader::applyUniform(modelMatrix);

		m_FullScreenQuad->setTexture(m_DirectionalLightFb->getTexture());
		m_FullScreenQuad->draw();
	}
	Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, true);

}

void TestApp::drawUi() {
	ShaderUniformData* uniformPVM;
	ShaderUniformData* uniformModel;
	ShaderUniformData* uniformColor;
	Transform model;

	float fontColor[] = { 0.7f,0.7f,0.7f,1.0f };


	Shader::use(m_TextShader);
	//get uniforms
	uniformColor = m_TextShader->m_CommonUniforms.m_Color;
	uniformPVM = m_TextShader->m_CommonUniforms.m_ProjectionViewMatrix;
	uniformModel = m_TextShader->m_CommonUniforms.m_ModelMatrix;

	//move ui up one line to show the new line
	model.setPosition(glm::vec3(0, m_Font->getLineOffset(1)+10, 0));

	uniformPVM->setData(&m_MainCamera->getProjectionViewMatrix());

	uniformModel->setData(&model);
	uniformColor->setData(fontColor);


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

	//testing the quick and not as efficient text rendering
	std::string quickText = "FPS: ";
	quickText += std::to_string(TimeHandler::getFps());
	m_Font->drawText(quickText.c_str());


	model.translate(glm::vec3(0, m_Font->getLineOffset(1), 0));

	uniformModel->setData(&model);
	Shader::applyUniform(uniformModel);

	//testing the quick and not as efficient text rendering
	quickText = "Spacing: ";
	quickText += std::to_string(m_Spacing);
	quickText += " Per Side: ";
	quickText += std::to_string(m_AmountPerSide);
	m_Font->drawText(quickText.c_str());
}

void TestApp::drawModels() {
	Transform model;
	float offset = m_Spacing*((m_AmountPerSide-1) / 2.0f);

	ShaderUniformData* modelMatrix = Shader::getCurrentShader()->m_CommonUniforms.m_ModelMatrix;
	ShaderUniformData* modelNormalRotMatrix = Shader::getCurrentShader()->m_CommonUniforms.m_NormalRotMatrix;

	glm::mat4 modelMat;
	glm::mat4 normalMat;

	int i = 0;
	for (int x = 0; x < m_AmountPerSide; x++) {
		for (int z = 0; z < m_AmountPerSide; z++) {

			model.setPosition(glm::vec3(-offset + m_Spacing * x, 0, offset + -m_Spacing * z));
			model.setRotation(glm::vec3(0, sin(TimeHandler::getCurrentTime()*0.25f + i) * 180 + 30 * i, 0));

			modelMat = model.getGlobalMatrix();
			normalMat = glm::transpose(glm::inverse(modelMat));
			if (modelMatrix != nullptr) {
				modelMatrix->setData(&modelMat);
				Shader::applyUniform(modelMatrix);
			}
			if (modelNormalRotMatrix != nullptr) {
				modelNormalRotMatrix->setData(&normalMat);
				Shader::applyUniform(modelNormalRotMatrix);
			}

			m_Model->draw();

			i++;
		}
	}
}
