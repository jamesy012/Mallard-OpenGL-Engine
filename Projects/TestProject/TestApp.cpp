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
	Logging::quickTimePush("StartUp");
	Logging::quickTimePush("FileLoading");

	//m_Model = new Model();
	//m_Model->load("Models/Nanosuit/nanosuit.obj");
	//m_Model->load("Models/stanford/Dragon.obj");
	//m_Model->load("Models/ModelTest/ModelTest.fbx");
	//m_Model->load("Models/ModelTest/ModelTest.obj");



	Model** models[4] = { &m_Model,&m_ModelShadow[0],&m_ModelShadow[1],&m_ModelShadow[2]};
	unsigned int* arrayBuffers[4] = { &m_InstanceArrayBuffer,
		&m_InstanceArrayBufferShadow[0],
		&m_InstanceArrayBufferShadow[1],
		&m_InstanceArrayBufferShadow[2] };
	glGenBuffers(1+NUM_OF_SHADOW_CASCADES, arrayBuffers[0]);
	for (int i = 0; i < 1 + NUM_OF_SHADOW_CASCADES; i++) {
		(*models[i]) = new Model();
		(*models[i])->load("Models/Nanosuit/nanosuit.obj");
		glBindBuffer(GL_ARRAY_BUFFER, *arrayBuffers[i]);
		for (int q = 0; q < (*models[i])->m_Meshs.size(); q++) {
			unsigned int VAO = (*models[i])->m_Meshs[q]->m_Vao;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*) 0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*) (sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*) (2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*) (3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	m_Font = new Font();
	m_Font->loadFont("c:/windows/fonts/comic.ttf", 48);
	//m_Font->loadFont("c:/windows/fonts/wingding.ttf", 48);

	Logging::quickTimePop(true);

	m_TestText = new Text(m_Font);
	m_TestText->generateText("Shadow Test\nMove: WASDQE, Rotate: Arrow keys, Other: ZXCVB, Culling: N");

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

	m_LightDir = glm::normalize(glm::vec3(1.0f, 2.5f, 1.0f));
	glm::mat4 lightView = glm::lookAt(m_LightDir, glm::vec3(0), glm::vec3(0, 1, 0));
	glm::mat4 lightProjection = glm::ortho<float>(-40, 40, -40, 40, -100, 100);

	for (int i = 0; i < NUM_OF_SHADOW_CASCADES; i++) {
		m_LightMatrix[i] = lightProjection * lightView;

		m_DirectionalLightFb[i] = new Framebuffer();
		if (i == 0) {
			m_DirectionalLightFb[i]->setSize(1024 * 2, 1024 * 2);
		} else {
			m_DirectionalLightFb[i]->setSize(1024, 1024);
		}
		m_DirectionalLightFb[i]->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::DEPTH);
		m_DirectionalLightFb[i]->genFramebuffer();
	}

	m_FirstModelUpdate = true;
	updateModel();
	Logging::quickTimePop(true);
}

void TestApp::shutDown() {
	m_Model->unload();
	for (int i = 0; i < 3; i++) {
		m_ModelShadow[i]->unload();
	}
	m_1x1WhiteTexture->unload();

	delete m_Font;
	delete m_TestText;
	delete m_TextShader;

	delete m_GroundPlane;

	delete m_ShadowGenerationShader;
	delete m_ShadowDrawShader;

	for (int i = 0; i < NUM_OF_SHADOW_CASCADES; i++) {
		delete m_DirectionalLightFb[i];
	}

	glDeleteBuffers(NUM_OF_SHADOW_CASCADES, &m_InstanceArrayBufferShadow[0]);
	glDeleteBuffers(1, &m_InstanceArrayBuffer);

	delete m_RenderingArray;
}

void TestApp::update() {
	float cameraMoveSpeed = 10.0f;
	float cameraRotateRpeed = 40.0f;
	if (Input::isKeyDown(KEY_E)) {
		m_GameCamera->m_Transform.translate(glm::vec3(0, cameraMoveSpeed, 0)*TimeHandler::getDeltaTime(), false);
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

	if (Input::isKeyRepeated(KEY_Z)) {
		m_AmountPerSide--;
		if (m_AmountPerSide < 0) {
			m_AmountPerSide = 0;
		}
		updateModel();
	}
	if (Input::isKeyRepeated(KEY_X)) {
		m_AmountPerSide++;
		updateModel();
	}

	if (Input::isKeyDown(KEY_C)) {
		m_Spacing -= 5 * TimeHandler::getDeltaTime();
		if (m_Spacing < 1) {
			m_Spacing = 1;
		}
		updateModel();
	}
	if (Input::isKeyDown(KEY_V)) {
		m_Spacing += 5 * TimeHandler::getDeltaTime();
		if (m_Spacing > 40) {
			m_Spacing = 40;
		}
		updateModel();
	}

	if (Input::wasKeyPressed(KEY_N)) {
		m_UseCulling = !m_UseCulling;
		updateModel();
	}


	if (Input::wasKeyPressed(KEY_B)) {
		const int sizes = 8;
		const int orthoSizes[sizes] = { 60,80,100,120,140,10,20,40 };
		static int currentSizeIndex = 0;
		int currSize = orthoSizes[currentSizeIndex++];
		if (currentSizeIndex >= sizes) {
			currentSizeIndex = 0;
		}
		m_CurrentOrthoSize = currSize;
	}

	if (Input::wasKeyPressed(KEY_O)) {
		m_ShadowGenerationShader->reloadShaders();
	}
	if (Input::wasKeyPressed(KEY_P)) {
		m_ShadowDrawShader->reloadShaders();
	}

	float time = TimeHandler::getCurrentTime() * 0.4f;
	m_LightDir = glm::normalize(glm::vec3(sin(time), 2.5f, cos(time)));

	glm::mat4 lightView = glm::lookAt(m_LightDir, glm::vec3(0), glm::vec3(0, 1, 0));
	lightView = glm::translate(lightView, m_GameCamera->m_Transform.getGlobalPosition() * -glm::vec3(1, 0, 1));

	for (int i = 0; i < NUM_OF_SHADOW_CASCADES; i++) {
		float orthoSize = m_CurrentOrthoSize*(i + 1);
		glm::mat4 lightProjection = glm::ortho<float>(-orthoSize, orthoSize,
													  -orthoSize, orthoSize,
													  -100, 100);
		m_LightMatrix[i] = lightProjection * lightView;
	}


	if (m_UseCulling) {
		if (m_GameCamera->m_Transform.getLastTransformUpdate() != m_LastFrustumUpdate) {
			getFrustumPlanes(m_GameCamera->getProjectionViewMatrix(), m_FrustumPlanes);
			m_LastFrustumUpdate = m_GameCamera->m_Transform.getLastTransformUpdate();
			updateModel();
		}

	}

	//if (TimeHandler::getCurrentFrameNumber() % 2 == 0) {
	//updateModel();
	//}
}

void TestApp::draw() {

	Transform model;
	{
		Shader::use(m_ShadowGenerationShader);
		ShaderUniformData* lightMatrix = m_ShadowGenerationShader->m_CommonUniforms.m_ProjectionViewMatrix;
		for (int i = 0; i < NUM_OF_SHADOW_CASCADES; i++) {
			Framebuffer::use(m_DirectionalLightFb[i]);
			Framebuffer::clearCurrentBuffer();

			lightMatrix->setData(&m_LightMatrix[i]);
			Shader::applyUniform(lightMatrix);

			//drawModels();
			m_ModelShadow[i]->drawInstance(m_NumberOfObjectsToRenderShadow[i]);
		}
	}
	Framebuffer::use(nullptr);
	{
		Shader::use(m_ShadowDrawShader);

		ShaderUniformData* cameraPvm = m_ShadowDrawShader->m_CommonUniforms.m_ProjectionViewMatrix;
		ShaderUniformData* modelMatrix = m_ShadowDrawShader->m_CommonUniforms.m_ModelMatrix;
		ShaderUniformData* modelNormalRotMatrix = m_ShadowDrawShader->m_CommonUniforms.m_NormalRotMatrix;
		ShaderUniformData* colorUniform = m_ShadowDrawShader->m_CommonUniforms.m_Color;
		ShaderUniformData* lightMatrix = m_ShadowDrawShader->getUniform(ShaderUniformTypes::MAT4, "lightMatrix");
		ShaderUniformData* lightDir = m_ShadowDrawShader->getUniform(ShaderUniformTypes::VEC3, "lightDir");
		ShaderUniformData* shadowMapTex = m_ShadowDrawShader->getUniform(ShaderUniformTypes::SAMPLER2D, "shadowMap");
		ShaderUniformData* useInstanced = m_ShadowDrawShader->getUniform(ShaderUniformTypes::FLOAT, "INSTANCED");
		float instancedOnOff[2] = { 1.0f,0.0f };

		glm::mat4 textureSpaceOffset = glm::mat4(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f
		);


		if (cameraPvm != nullptr) {
			cameraPvm->setData(&m_GameCamera->getProjectionViewMatrix());
			Shader::applyUniform(cameraPvm);
		}
		if (lightDir != nullptr) {
			lightDir->setData(glm::value_ptr(m_LightDir));
			Shader::applyUniform(lightDir);
		}
		if (lightMatrix != nullptr) {
			//lightMatrix->setData(&offsetLightMatrix);
			//Shader::applyUniform(lightMatrix);
		}

		for (int i = 0; i < NUM_OF_SHADOW_CASCADES; i++) {
			int slot = 3 + i;
			m_DirectionalLightFb[i]->getTexture()->bindTexture(slot);

			std::string shadowMapUniName = "shadowMap[" + std::to_string(i) + "]";
			std::string lightMatrixUniName = "lightMatrix[" + std::to_string(i) + "]";

			int loc = glGetUniformLocation(Shader::getCurrentShader()->getProgram(), shadowMapUniName.c_str());
			glUniform1i(loc, slot);
			glm::mat4 offsetLightMatrix = textureSpaceOffset * m_LightMatrix[i];

			loc = glGetUniformLocation(Shader::getCurrentShader()->getProgram(), lightMatrixUniName.c_str());
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(offsetLightMatrix));

		}

		useInstanced->setData(&instancedOnOff[0]);
		Shader::applyUniform(useInstanced);

		if (colorUniform != nullptr) {
			glm::vec4 white = glm::vec4(1);
			colorUniform->setData(glm::value_ptr(white));
			Shader::applyUniform(colorUniform);
		}

		//drawModels();
		m_Model->drawInstance(m_AmountRendering);

		model.setPosition(m_GameCamera->m_Transform.getGlobalPosition() * glm::vec3(1, 0, 1));

		model.setScale(200);
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

		if (colorUniform != nullptr) {
			glm::vec4 green = glm::vec4(0.25f, 1.0f, 0.25f, 1.0f);
			colorUniform->setData(glm::value_ptr(green));
			Shader::applyUniform(colorUniform);
		}

		useInstanced->setData(&instancedOnOff[1]);
		Shader::applyUniform(useInstanced);
		m_GroundPlane->draw();

		for (int i = 0; i < NUM_OF_SHADOW_CASCADES; i++) {
			glActiveTexture(GL_TEXTURE0 + 3 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	{
		Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, false);

		Shader::use(m_BasicShader);

		ShaderUniformData* cameraPvm = m_BasicShader->m_CommonUniforms.m_ProjectionViewMatrix;
		ShaderUniformData* modelMatrix = m_BasicShader->m_CommonUniforms.m_ModelMatrix;

		model = m_MainCamera->m_Transform;
		model.translate(glm::vec3(-15, 0, -20), false);
		model.rotate(glm::vec3(90, 0, 0));
		model.setScale(3);

		cameraPvm->setData(&m_GameCamera->getProjectionViewMatrix());
		Shader::applyUniform(cameraPvm);
		modelMatrix->setData(&model);
		Shader::applyUniform(modelMatrix);

		static int value = 0;
		if (Input::wasKeyPressed(KEY_1)) {
			value++;
			if (value >= NUM_OF_SHADOW_CASCADES) {
				value = 0;
			}
		}

		m_FullScreenQuad->setTexture(m_DirectionalLightFb[value]->getTexture());
		m_FullScreenQuad->draw();

		Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, true);
	}

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
	model.setPosition(glm::vec3(0, m_Font->getLineOffset(1) + 10, 0));

	uniformPVM->setData(&m_MainCamera->getProjectionViewMatrix());

	uniformModel->setData(&model);
	uniformColor->setData(fontColor);

	Shader::applyUniform(uniformPVM);
	Shader::applyUniform(uniformColor);
	Shader::applyUniform(uniformModel);

	m_TestText->draw();

	std::string quickText;
	//fps
	{
		//move the position up 1 more line for the quick draw
		model.translate(glm::vec3(0, m_Font->getLineOffset(1), 0));

		uniformModel->setData(&model);
		Shader::applyUniform(uniformModel);

		//testing the quick and not as efficient text rendering
		quickText = "FPS: ";
		quickText += std::to_string(TimeHandler::getFps());
		m_Font->drawText(quickText.c_str());
	}

	{
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

	if (m_UseCulling) {
		model.translate(glm::vec3(0, m_Font->getLineOffset(1), 0));

		uniformModel->setData(&model);
		Shader::applyUniform(uniformModel);

		//testing the quick and not as efficient text rendering
		quickText = "Rendering: View: ";
		quickText += std::to_string(m_AmountRendering);
		quickText += " Culling Checks: ";
		quickText += std::to_string(m_NumberOfFrustumChecks);
		m_Font->drawText(quickText.c_str());
	}

	{
		model.translate(glm::vec3(0, m_Font->getLineOffset(1), 0));

		uniformModel->setData(&model);
		Shader::applyUniform(uniformModel);

		//testing the quick and not as efficient text rendering
		quickText = "Info: Vertices Rendered: ";
		quickText += std::to_string(Logging::getNumVerticesRendered());
		m_Font->drawText(quickText.c_str());
	}
}

void TestApp::updateModel() {
	printf("Model Update %i\n", TimeHandler::getCurrentFrameNumber());
	Logging::quickTimePush("UpdateMode", true);
	Logging::quickTimePush("UpdateMode::StartUp", false);
	Transform model;
	float offset = m_Spacing*((m_AmountPerSide - 1) / 2.0f);

	//ShaderUniformData* modelMatrix = Shader::getCurrentShader()->m_CommonUniforms.m_ModelMatrix;
	//ShaderUniformData* modelNormalRotMatrix = Shader::getCurrentShader()->m_CommonUniforms.m_NormalRotMatrix;

	glm::mat4 modelMat;
	//glm::mat4 normalMat;

	int renderingBefore = m_AmountRendering;
	m_AmountRendering = 0;
	m_NumberOfFrustumChecks = 0;

	if (m_RenderingArray != nullptr) {
		delete m_RenderingArray;
	}
	m_RenderingArray = new bool[m_AmountPerSide*m_AmountPerSide];

	Logging::quickTimePop(false);
	Logging::quickTimePush("UpdateMode::CullingCheck", false);
	if (m_UseCulling) {
		int i = -1;
		for (int x = 0; x < m_AmountPerSide; x++) {
			for (int z = 0; z < m_AmountPerSide; z++) {
				i++;

				model.setPosition(glm::vec3(-offset + m_Spacing * x, 0, offset + -m_Spacing * z));

				if (m_UseCulling) {
					glm::vec3 pos = model.getLocalPosition();

					bool result = checkFustumPlanes(pos, m_FrustumPlanes) || checkFustumPlanes(pos + glm::vec3(0, 20, 0), m_FrustumPlanes);
					m_RenderingArray[i] = result;

					if (!result) {
						continue;
					}

					m_AmountRendering++;
				}
			}
		}
	} else {
		m_AmountRendering = m_AmountPerSide*m_AmountPerSide;
	}
	if (m_AmountRendering == 0) {
		Logging::quickTimePop(false);
		Logging::quickTimePop(true);
		return;
	}

	Logging::quickTimePop(false);
	Logging::quickTimePush("UpdateMode::GenModelMarices", false);

	glm::mat4* modelMatrices = new glm::mat4[m_AmountRendering];
	int arrayIndex = 0;

	int i = -1;
	for (int x = 0; x < m_AmountPerSide; x++) {
		for (int z = 0; z < m_AmountPerSide; z++) {
			i++;

			if (!m_RenderingArray[i] && m_UseCulling) {
				continue;
			}

			model.setPosition(glm::vec3(-offset + m_Spacing * x, 0, offset + -m_Spacing * z));
			model.setRotation(glm::vec3(0, sin(17.25f * i) * 180 + 30 * i, 0));


			modelMat = model.getLocalMatrix();
			modelMatrices[arrayIndex++] = modelMat;

		}
	}
	Logging::quickTimePop(false);
	Logging::quickTimePush("UpdateMode::BufferSetting", false);

	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, (m_AmountRendering) * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	Logging::quickTimePop(false);
	Logging::quickTimePush("UpdateMode::End", false);

	delete modelMatrices;

	Logging::quickTimePop(false);
	Logging::quickTimePop(true);

	for (int i = 0; i < NUM_OF_SHADOW_CASCADES; i++) {
		updateShadowModel(i);
	}
}

void TestApp::updateShadowModel(unsigned int a_ShadowCascade) {
	glm::vec4 frustumPlanes[6];
	getFrustumPlanes(m_LightMatrix[a_ShadowCascade], frustumPlanes);

	Transform model;
	float offset = m_Spacing*((m_AmountPerSide - 1) / 2.0f);

	glm::mat4 modelMat;
	//glm::mat4 normalMat;

	int numToRender = 0;

	bool* renderingArray = new bool[m_AmountPerSide*m_AmountPerSide]{ false };

	if (m_UseCulling) {
		int i = -1;
		for (int x = 0; x < m_AmountPerSide; x++) {
			for (int z = 0; z < m_AmountPerSide; z++) {
				i++;

				if (!m_RenderingArray[i]) {
					continue;
				}

				model.setPosition(glm::vec3(-offset + m_Spacing * x, 0, offset + -m_Spacing * z));

				glm::vec3 pos = model.getLocalPosition();

				bool result = checkFustumPlanes(pos + glm::vec3(0, 10, 0), frustumPlanes);
				renderingArray[i] = result;

				if (!result) {
					continue;
				}

				numToRender++;

			}
		}
	} else {
		numToRender = m_AmountPerSide*m_AmountPerSide;
	}
	if (numToRender == 0) {
		delete renderingArray;
		return;
	}

	glm::mat4* modelMatrices = new glm::mat4[numToRender];
	int arrayIndex = 0;

	int i = -1;
	for (int x = 0; x < m_AmountPerSide; x++) {
		for (int z = 0; z < m_AmountPerSide; z++) {
			i++;

			if ((!renderingArray[i]) && m_UseCulling) {
				continue;
			}

			model.setPosition(glm::vec3(-offset + m_Spacing * x, 0, offset + -m_Spacing * z));
			model.setRotation(glm::vec3(0, sin(17.25f * i) * 180 + 30 * i, 0));


			modelMat = model.getLocalMatrix();
			modelMatrices[arrayIndex++] = modelMat;

		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceArrayBufferShadow[a_ShadowCascade]);
	glBufferData(GL_ARRAY_BUFFER, (numToRender) * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	delete renderingArray;
	delete modelMatrices;

	m_NumberOfObjectsToRenderShadow[a_ShadowCascade] = numToRender;
}

void TestApp::getFrustumPlanes(const glm::mat4 & a_Transform, glm::vec4* a_Frustums) {
	for (int i = 0; i < 6; i += 2) {
		int index = i / 2;
		a_Frustums[i] = glm::vec4(
			a_Transform[0][3] - a_Transform[0][index],
			a_Transform[1][3] - a_Transform[1][index],
			a_Transform[2][3] - a_Transform[2][index],
			a_Transform[3][3] - a_Transform[3][index]
		);
		a_Frustums[i + 1] = glm::vec4(
			a_Transform[0][3] + a_Transform[0][index],
			a_Transform[1][3] + a_Transform[1][index],
			a_Transform[2][3] + a_Transform[2][index],
			a_Transform[3][3] + a_Transform[3][index]
		);
	}

	//normalize planes
	for (int i = 0; i < 6; i++) {
		float d = glm::length(glm::vec3(a_Frustums[i]));
		a_Frustums[i] /= d;
	}
}

bool TestApp::checkFustumPlanes(const glm::vec3 a_Position, glm::vec4* a_Frustums) {
	m_NumberOfFrustumChecks++;
	for (int i = 0; i < 6; i++) {
		float d = glm::dot(glm::vec3(a_Frustums[i]), a_Position) + a_Frustums[i].w;

		if (d <= -2) {
			return false;
		}

	}
	return true;
}
