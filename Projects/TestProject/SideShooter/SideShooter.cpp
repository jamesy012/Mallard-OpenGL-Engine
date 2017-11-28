#include "SideShooter.h"

#include <stdio.h>
#include <algorithm>

#include <gl\glew.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "TimeHandler.h"
#include "Window.h"
#include "Input.h"
#include "Keys.h"
#include "Logging.h"
#include "Framebuffer.h"

#include "Skybox.h"
#include "Shader.h"
#include "Camera.h"
#include "Font.h"
#include "Text.h"
#include "Texture.h"

#include "Model.h"
#include "Mesh.h"
#include "Transform.h"
#include "Object.h"

#include "Helpers.h"
#include "Projectile.h"
#include "Player.h"
#include "SideShooterCamera.h"
#include "SideShooterContants.h"
#include "EnemySpawner.h"

#include "Enemys\EnemyStationary.h"


static bool objectDistSort(glm::mat4& a, glm::mat4& b) {
	return glm::value_ptr(a)[14] > glm::value_ptr(b)[14];
}

void SideShooter::startUp() {
	//text
	{
		m_TestText = new Text(m_Font);
		m_TestText->generateText(R"(Side Shooter
		Move: WASD, Shoot: Space)");
	}
	//shaders
	{
		m_UniformShader = new Shader();
		m_UniformShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/BasicInstancing.vert");
		m_UniformShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/BasicInstancing.frag");
		m_UniformShader->linkShader();

		m_ReflectionShader = new Shader();
		m_ReflectionShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/Reflection.vert");
		m_ReflectionShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/Reflection.frag");
		m_ReflectionShader->linkShader();

		m_BlurShader = new Shader();
		m_BlurShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
		m_BlurShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/GaussianBlur.frag");
		m_BlurShader->linkShader();

		m_DOFGenShader = new Shader();
		m_DOFGenShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
		m_DOFGenShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/DOFGen.frag");
		m_DOFGenShader->linkShader();
		m_DOFDrawShader = new Shader();
		m_DOFDrawShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
		m_DOFDrawShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/DOFDraw.frag");
		m_DOFDrawShader->linkShader();
	}
	//framebuffers
	{
		m_DepthOfFieldTest = new Framebuffer();
		m_DepthOfFieldTest->setSize(1024, 1024);
		m_DepthOfFieldTest->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGBA, 16);
		m_DepthOfFieldTest->genFramebuffer();
	}
	//textures
	{
		m_GrassTexture = new Texture();
		m_GrassTexture->load("Models/SideShooter/Grass.jpg");
	}
	//model/meshs
	{
		m_TreeModel = new Model();
		m_TreeModel->load("Models/SideShooter/lowpolytree.obj");
		//m_TreeModel->m_Transform.setScale(0.05f);

		m_Box = new Mesh();
		m_Box->createBox();

		m_QuadMesh = new Model();
		m_QuadMesh->load("Models/SideShooter/UvScaledPlane.obj");
		m_QuadMesh->m_Meshs[0]->setTexture(m_GrassTexture);

		m_PondMesh = new Model();
		m_PondMesh->load("Models/SideShooter/Pond.obj");
	}


	//objects
	{
		m_Ground = new Object("Ground Plane");
		m_Ground->m_Renderable = m_QuadMesh;
		m_Ground->m_Transform.setPosition(glm::vec3(0, -SSConstants::GROUND_Y, -150));
		m_Ground->m_Transform.setScale(SSConstants::GAME_WIDTH / 100.0f);

		//had to create player first, then the camera to give it the reference
		//then set the camera in the player
		m_Player = new Player();
		m_Camera = new SideShooterCamera(m_Player);

		m_Camera->setPerspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
		m_Player->m_Camera = m_Camera;

		for (int i = 0; i < NUM_OF_PONDS; i++) {
			Object* pond = new Object();
			m_Ponds[i] = pond;
			pond->m_Renderable = m_PondMesh;
			pond->m_Transform.setScale(glm::vec3(m_PondSize, 1, m_PondSize));
			pond->m_Transform.setPosition(
				glm::vec3(
				getRandomWithinRange(-SSConstants::GAME_WIDTH, SSConstants::GAME_WIDTH),
				-SSConstants::GROUND_Y + 0.5f,
				-getRandomWithinRange(-50, 200)));
			float xzRot = 8.0f;
			pond->m_Transform.setRotation(
				glm::vec3(
				0,
				getRandomWithinRange(0, 360),
				0));
		}
	}

	//reflection
	{
		m_ReflectionCamera = new Camera();
		m_ReflectionCamera->setPerspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

		m_ReflectionBuffer = new Framebuffer();
		m_ReflectionBuffer->setSize(1024, 1024);
		m_ReflectionBuffer->createRenderTarget();

		m_ReflectionScaledBuffer = new Framebuffer();
		//m_ReflectionScaledBuffer->setSize(256, 256);
		m_ReflectionScaledBuffer->setSize(1024, 1024);
		m_ReflectionScaledBuffer->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGBA, 16);
		m_ReflectionScaledBuffer->genFramebuffer();

		m_PondMesh->m_Meshs[0]->setTexture(m_ReflectionBuffer->getTexture(0));
	}

	m_EnemySpawner = new EnemySpawner(this, m_Player);

	//map generation
	{
		//glm::vec3 pondPos = m_Pond->m_Transform.getLocalPosition();
		for (int i = 0; i < NUM_OF_TREES; i++) {
			Transform t;
			glm::vec3 treePos = glm::vec3(
				getRandomWithinRange(-SSConstants::GAME_WIDTH, SSConstants::GAME_WIDTH),
				-SSConstants::GROUND_Y,
				-getRandomWithinRange(0, 300) - 10);
			bool failedPondTest = false;
			for (int q = 0; q < NUM_OF_PONDS; q++) {
				if (glm::distance(m_Ponds[q]->m_Transform.getLocalPosition(), treePos) < m_PondSize + 10) {
					failedPondTest = true;
					break;
				}
			}
			if (failedPondTest) {
				m_NumofTreesGenerated--;
				i--;
				continue;
			}
			t.setPosition(treePos);
			t.setScale(getRandomWithinRange(2, 5));
			float xzRot = 8.0f;
			t.setRotation(
				glm::vec3(
				getRandomWithinRange(-xzRot, xzRot),
				getRandomWithinRange(0, 360),
				getRandomWithinRange(-xzRot, xzRot)));
			m_UniformTrees[i] = t.getGlobalMatrix();
		}
		std::sort(std::begin(m_UniformTrees), std::end(m_UniformTrees), objectDistSort);
	}
}

void SideShooter::shutDown() {
	m_TreeModel->unload();
	m_QuadMesh->unload();
	m_GrassTexture->unload();
	m_PondMesh->unload();

	delete m_UniformShader;

	delete m_Box;
	delete m_Player;
	delete m_Ground;

	for (int i = 0; i < NUM_OF_PONDS; i++) {
		delete m_Ponds[i];
	}
	delete m_TestText;

	delete m_Camera;

	delete m_ReflectionBuffer;
	delete m_ReflectionCamera;
	delete m_ReflectionShader;
	delete m_ReflectionScaledBuffer;
	delete m_BlurShader;

	delete m_DOFGenShader;
	delete m_DOFDrawShader;
	delete m_DepthOfFieldTest;



	for (unsigned int i = 0; i < MAX_NUM_OF_PROJECTILES; i++) {
		if (m_Projectiles[i] != nullptr) {
			delete m_Projectiles[i];
		}
	}

	for (unsigned int i = 0; i < MAX_NUM_OF_ENEMIES; i++) {
		if (m_Enemies[i] != nullptr) {
			delete (EnemyStationary*) m_Enemies[i];
		}
	}

	delete m_EnemySpawner;
}

void SideShooter::update() {
	if (Input::wasKeyPressed(KEY_Q)) {
		m_DOFDrawShader->reloadShaders();
	}
	if (Input::wasKeyPressed(KEY_E)) {
		m_DOFGenShader->reloadShaders();
	}
	if (Input::wasKeyPressed(KEY_R)) {
		m_ReflectionShader->reloadShaders();
	}

	m_AppOptions.m_EnableDof ^= Input::wasKeyPressed(KEY_KP_1);
	m_AppOptions.m_DisplayUI ^= Input::wasKeyPressed(KEY_KP_2);
	m_Flags.m_RunDebugTimers ^= Input::wasKeyPressed(KEY_F1);

	//adjust dof
	if (m_IsDoingDofIntro) {
		if (m_DofChangeStartTime < 0) {
			m_DofChangeStartTime = TimeHandler::getCurrentTime();
		} else {
			float percentage = (TimeHandler::getCurrentTime() - m_DofChangeStartTime) / m_DofChangeDuration;
			if (percentage > 1) {
				m_IsDoingDofIntro = false;
				percentage = 1;
			}
			ShaderUniformData* fDist = m_DOFGenShader->getUniform(ShaderUniformTypes::FLOAT, "focusDistance");
			float value = lerp(m_StartingDof, m_TargetDof, percentage);
			fDist->setData(&value);
		}
	} else {
		int keys = 0;
		keys |= Input::isKeyDown(KEY_KP_ADD) << 0;
		keys |= Input::isKeyDown(KEY_KP_SUBTRACT) << 1;
		if (keys != 0) {
			ShaderUniformData* fDist = m_DOFGenShader->getUniform(ShaderUniformTypes::FLOAT, "focusDistance");
			float value = *(float*) fDist->getDataVoid();
			float increment = 50 * TimeHandler::getDeltaTime();
			if (keys == 1) {
				value += increment;
				if (value > 1000) {
					value = 1000;
				}
			}
			if (keys == 2) {
				value -= increment;
				if (value < 0) {
					value = 0;
				}
			}
			fDist->setData(&value);
		}
	}

	m_Player->update();

	if (Input::wasKeyPressed(KEY_SPACE)) {
		spwanProjectile(&m_Player->m_ShootPoint, m_Player->m_DirectionFacing > 0, m_Box);

	}

	for (unsigned int i = 0; i < MAX_NUM_OF_PROJECTILES; i++) {
		if (m_Projectiles[i] != nullptr) {
			m_Projectiles[i]->update();
			if (m_Projectiles[i]->m_ShouldDelete) {
				delete m_Projectiles[i];
				m_Projectiles[i] = nullptr;
			}
		}
	}

	m_NumOfEnemiesAlive = 0;
	for (unsigned int i = 0; i < MAX_NUM_OF_ENEMIES; i++) {
		if (m_Enemies[i] != nullptr) {
			m_Enemies[i]->update();
			m_NumOfEnemiesAlive++;
		}
	}

	//Collision check 
	runCollisionCheck();

	m_EnemySpawner->update();

	m_Camera->update();

	float scale = ((m_Player->m_Movement.x * 1.25f) / m_Player->m_MovementSpeed) * -10;
	m_CameraGame->m_Transform.rotate(glm::vec3(0, scale, 0) * TimeHandler::getDeltaTime());

	glm::vec3 camPos = m_Camera->m_Transform.getGlobalPosition();
	float camY = camPos.y;
	camPos.y = -SSConstants::GROUND_Y - (camPos.y + SSConstants::GROUND_Y);
	glm::vec3 camRot = m_Camera->m_Transform.getGlobalRotationEulers();
	camRot *= glm::vec3(-1, 1, -1);
	m_ReflectionCamera->m_Transform.setPosition(camPos);
	m_ReflectionCamera->m_Transform.setRotation(camRot);
	m_ReflectionCamera->m_Transform.setScale(glm::vec3(1, -1, 1));

	//m_CameraGame->m_Transform.setPosition(glm::vec3(0, m_Camera->m_Transform.getLocalPosition().y, 0));

}

void SideShooter::draw() {
	if (m_DebugRunTimers) {
		Logging::quickTimePush("Reflection Render");
	}
	Framebuffer::use(m_ReflectionBuffer);
	Framebuffer::clearCurrentBuffer();

	//reflection skybox
	{
		Camera skyboxRefCam = *m_CameraGame;
		skyboxRefCam.m_Transform.setPosition(glm::vec3(0, SSConstants::GROUND_Y / 2, 0));
		//glm::vec3 camRot = m_CameraGame->m_Transform.getGlobalRotationEulers();
		//camRot *= glm::vec3(-1, 1, -1);
		//skyboxRefCam.m_Transform.setRotation(camRot);
		m_SkyboxGame->draw(skyboxRefCam);
	}
	glDisable(GL_CULL_FACE);

	sceneRender(m_ReflectionCamera, true);

	glEnable(GL_CULL_FACE);

	if (m_DebugRunTimers) {
		Logging::quickTimePop(true);
		Logging::quickTimePush("Blur Render");
	}
	{
		Framebuffer::use(m_ReflectionScaledBuffer);
		Framebuffer::clearCurrentBuffer(true, false);
		Shader::use(m_BlurShader);
		ShaderUniformData* radiusUniform = m_BlurShader->getUniform(ShaderUniformTypes::FLOAT, "radius");
		ShaderUniformData* dirUniform = m_BlurShader->getUniform(ShaderUniformTypes::VEC2, "dir");

		glm::vec2 dir[2] = { glm::vec2(1,0),glm::vec2(0,1) };
		float radius[2] = { .3,.45 };
		dirUniform->setData(&dir[0]);
		radiusUniform->setData(&radius[0]);
		//float mouseX = Input::getMouseX() / (float) Window::getMainWindow()->getFramebufferWidth();
		//radiusUniform->setData(&mouseX);

		Shader::checkUniformChanges();

		m_FullScreenQuad->setTexture(m_ReflectionBuffer->getTexture());
		m_FullScreenQuad->draw();

		Framebuffer::use(m_ReflectionBuffer);

		dirUniform->setData(&dir[1]);
		radiusUniform->setData(&radius[1]);

		//float mouseY = Input::getMouseY() / (float) Window::getMainWindow()->getFramebufferHeight();
		//radiusUniform->setData(&mouseY);

		Shader::checkUniformChanges();

		Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, false);
		m_FullScreenQuad->setTexture(m_ReflectionScaledBuffer->getTexture());
		m_FullScreenQuad->draw();
		Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, true);
	}
	if (m_DebugRunTimers) {
		Logging::quickTimePop(true);
		Logging::quickTimePush("Normal Scene Render");
	}

	//Framebuffer::use(m_DepthOfFieldTest);
	//Framebuffer::clearCurrentBuffer();
	Framebuffer::use(nullptr);

	//render ground
	if (m_DebugRunTimers) {
		Logging::quickTimePush("Normal Scene Render - ground");
	}

	{
		Shader::use(m_ShaderBasic);

		ShaderUniformData* uniformPvm = m_ShaderBasic->m_CommonUniforms.m_ProjectionViewMatrix;
		uniformPvm->setData(m_Camera);
		Shader::applyUniform(uniformPvm);

		drawObject(m_Ground, false);
	}

	if (m_DebugRunTimers) {
		Logging::quickTimePop(true);
		Logging::quickTimePush("Normal Scene Render - rest of the scene");
	}

	sceneRender(m_Camera, false);

	//render reflection pond
	if (m_DebugRunTimers) {
		Logging::quickTimePop(true);
		Logging::quickTimePush("Normal Scene Render - pond reflection");
	}
	{
		glEnable(GL_BLEND);

		//glDepthMask(false);
		glDepthFunc(GL_LEQUAL);
		Shader::use(m_ReflectionShader);

		ShaderUniformData* uniformPvm = m_ReflectionShader->m_CommonUniforms.m_ProjectionViewMatrix;
		uniformPvm->setData(m_Camera);
		Shader::applyUniform(uniformPvm);

		//m_QuadMesh->m_Meshs[0]->setTexture(m_ReflectionBuffer->getTexture());
		for (int i = 0; i < NUM_OF_PONDS; i++) {
			drawObject(m_Ponds[i], true, m_PondSize);
		}
		glDepthFunc(GL_LESS);
		//glDepthMask(true);
		glDisable(GL_BLEND);
	}


	if (m_DebugRunTimers) {
		Logging::quickTimePop(true);
	}
	if (m_AppOptions.m_EnableDof) {
		if (m_DebugRunTimers) {
			Logging::quickTimePush("Normal Scene Render - DOF");
		}
		Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, false);

		int width = Window::getMainWindow()->getFramebufferWidth();
		int height = Window::getMainWindow()->getFramebufferHeight();
		if (m_ReflectionScaledBuffer->getFramebufferWidth() != width || m_ReflectionScaledBuffer->getFramebufferHeight() != height) {
			m_ReflectionScaledBuffer->resizeFramebuffer(width, height);
		}
		if (m_DepthOfFieldTest->getFramebufferWidth() != width || m_DepthOfFieldTest->getFramebufferHeight() != height) {
			m_DepthOfFieldTest->resizeFramebuffer(width, height);
		}

		Shader::use(m_DOFGenShader);
		Framebuffer::use(m_DepthOfFieldTest);
		Framebuffer::clearCurrentBuffer(true, true);

		m_DOFGenShader->checkUniformChanges();

		m_FullScreenQuad->setTexture(m_FbGameFrame->getTexture(1));
		m_FullScreenQuad->draw();


		Framebuffer::use(m_ReflectionScaledBuffer);
		Framebuffer::clearCurrentBuffer(true, true);

		Shader::use(m_DOFDrawShader);

		m_FbGameFrame->getTexture(0)->bindTexture(1);
		m_DepthOfFieldTest->getTexture()->bindTexture(2);


		ShaderUniformData* radiusUniform = m_DOFDrawShader->getUniform(ShaderUniformTypes::FLOAT, "radius");
		ShaderUniformData* dirUniform = m_DOFDrawShader->getUniform(ShaderUniformTypes::VEC2, "dir");
		glm::vec2 dir[2] = { glm::vec2(1,0),glm::vec2(0,1) };
		float radius[2] = { .2,.2 };

		int loc = glGetUniformLocation(Shader::getCurrentShader()->getProgram(), "TexDiffuse1");
		glUniform1i(loc, 1);
		loc = glGetUniformLocation(Shader::getCurrentShader()->getProgram(), "depthTexture");
		glUniform1i(loc, 2);

		if (dirUniform != nullptr) {
			dirUniform->setData(&dir[0]);
			radiusUniform->setData(&radius[0]);
		}

		Shader::checkUniformChanges();

		m_FullScreenQuad->setTexture(nullptr);
		m_FullScreenQuad->draw();

		Framebuffer::use(nullptr);
		//Framebuffer::clearCurrentBuffer();

		m_ReflectionScaledBuffer->getTexture(0)->bindTexture(1);

		if (dirUniform != nullptr) {
			dirUniform->setData(&dir[1]);
			radiusUniform->setData(&radius[1]);
		}

		Shader::checkUniformChanges();

		m_FullScreenQuad->setTexture(nullptr);
		m_FullScreenQuad->draw();


		//unbind texture
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, 0);

		Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, true);
		if (m_DebugRunTimers) {
			Logging::quickTimePop(true);
		}
	}//dof end

	if (m_DebugRunTimers) {
		Logging::quickTimePop(true);
	}
}

void SideShooter::drawUi() {
	//if we dont want to display the UI then return
	if (!m_AppOptions.m_DisplayUI) {
		return;
	}
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
	model.translate(glm::vec3(0, Window::getMainWindow()->getWindowHeight() + -m_Font->getLineOffset(m_TestText->getNumberOfLines() - 1), 0));

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

Projectile* SideShooter::spwanProjectile(Transform * a_Position, bool a_Right, IRenderable* a_Model) {
	//find next empty spot
	for (unsigned int i = 0; i < MAX_NUM_OF_PROJECTILES; i++) {
		if (m_Projectiles[i] == nullptr) {
			m_Projectiles[i] = new Projectile(a_Right);
			m_Projectiles[i]->m_Renderable = a_Model;
			m_Projectiles[i]->m_Camera = m_Camera;
			m_Projectiles[i]->m_Transform.setPosition(a_Position->getGlobalPosition());
			return m_Projectiles[i];
		}
	}
	return nullptr;
}

Enemy* SideShooter::spawnEnemy(glm::vec3 a_Position) {
	for (unsigned int i = 0; i < MAX_NUM_OF_ENEMIES; i++) {
		if (m_Enemies[i] == nullptr) {
			m_Enemies[i] = new EnemyStationary(nullptr, m_Player, a_Position, m_Box, this);
			return m_Enemies[i];
		}
	}
	return nullptr;
}

void SideShooter::drawObjectInstanced(IRenderable * a_Renderable, glm::mat4 * a_Array, int a_ArraySize) {
	ShaderUniformData* uniformOffset = m_UniformShader->getUniform(ShaderUniformTypes::VEC4, "offset");
	int loc = glGetUniformLocation(Shader::getCurrentShader()->getProgram(), "models[0]");

	glm::vec4 offset = glm::vec4(0);
	const int MAX_INSTANCES_PER_DRAW = 128;
	float batches = a_ArraySize / (float) MAX_INSTANCES_PER_DRAW;
	float batchesAmount = batches;
	int amountToRender = MAX_INSTANCES_PER_DRAW;
	for (int i = 0; i < batchesAmount; i++) {
		if (batches < 1.0f) {
			amountToRender = a_ArraySize % MAX_INSTANCES_PER_DRAW;
		}
		batches -= 1;

		glUniformMatrix4fv(loc, amountToRender, GL_FALSE, glm::value_ptr(a_Array[i * MAX_INSTANCES_PER_DRAW]));

		for (int q = -1; q < 2; q++) {
			offset.x = SSConstants::GAME_WIDTH * 2 * q;
			uniformOffset->setData(glm::value_ptr(offset));
			Shader::applyUniform(uniformOffset);
			a_Renderable->drawInstance(amountToRender);
		}
	}
}

void SideShooter::drawObject(IRenderable* a_Renderable, bool a_Cull, float a_CullOffset) {

	glm::vec3 startingPos = a_Renderable->m_Transform.getGlobalPosition();

	for (int i = -1; i < 2; i++) {
		glm::vec3 newPos = startingPos + glm::vec3(SSConstants::GAME_WIDTH * 2 * i, 0, 0);

		if (a_Cull) {
			float dist = abs(newPos.x - m_Camera->m_Transform.getLocalPosition().x) + newPos.z;
			if (dist > SSConstants::CLOSE_RENDER + a_CullOffset) {
				continue;
			}
		}

		a_Renderable->m_Transform.setPosition(newPos);
		a_Renderable->draw();
	}
	a_Renderable->m_Transform.setPosition(startingPos);

}

void SideShooter::runCollisionCheck() {
	for (unsigned int i = 0; i < MAX_NUM_OF_PROJECTILES; i++) {
		bool didHit = false;
		if (m_Projectiles[i] != nullptr) {
			glm::vec3 projPos = m_Projectiles[i]->getPositionCombined();
			for (unsigned int q = 0; q < MAX_NUM_OF_ENEMIES; q++) {
				if (m_Enemies[q] != nullptr) {
					glm::vec3 dist = projPos - m_Enemies[q]->getPositionCombined();
					if (abs(dist.x) < m_Projectiles[i]->m_ProjectileSize &&
						abs(dist.y) < m_Projectiles[i]->m_ProjectileSize) {

						//todo spawn explosion particle?

						didHit = true;
						delete m_Enemies[q];
						m_Enemies[q] = nullptr;
						break;
					}
				}
			}
			if (didHit) {
				delete m_Projectiles[i];
				m_Projectiles[i] = nullptr;
			}
		}
	}
}

void SideShooter::sceneRender(Camera * a_Camera, bool a_CloseOnly) {
	{
		Shader::use(m_UniformShader);

		ShaderUniformData* uniformPvm = m_UniformShader->m_CommonUniforms.m_ProjectionViewMatrix;
		uniformPvm->setData(a_Camera);
		Shader::applyUniform(uniformPvm);
		Logging::quickGpuDebugGroupPush("TREE RENDER1");

		if (a_CloseOnly) {
			int maxAmount = std::min(128u, m_NumofTreesGenerated);
			drawObjectInstanced(m_TreeModel, &m_UniformTrees[0], maxAmount);
		} else {
			drawObjectInstanced(m_TreeModel, &m_UniformTrees[0], m_NumofTreesGenerated);
		}

		Logging::quickGpuDebugGroupPop();

		if (m_NumOfEnemiesAlive != 0) {
			glm::mat4* enemies = new glm::mat4[m_NumOfEnemiesAlive];
			int index = 0;
			for (unsigned int i = 0; i < MAX_NUM_OF_ENEMIES; i++) {
				if (m_Enemies[i] != nullptr) {
					enemies[index++] = m_Enemies[i]->getGlobalMatrixCombined();
					if (index > m_NumOfEnemiesAlive) {
						break;
					}
				}
			}
			drawObjectInstanced(m_EnemySpawner->m_EnemyStationaryMesh, &enemies[0], m_NumOfEnemiesAlive);

			delete enemies;
		}
		{
			glm::mat4 projectiles[MAX_NUM_OF_PROJECTILES];
			int index = 0;
			for (unsigned int i = 0; i < MAX_NUM_OF_PROJECTILES; i++) {
				if (m_Projectiles[i] != nullptr) {
					projectiles[index++] = m_Projectiles[i]->getGlobalMatrixCombined();
				}
			}
			drawObjectInstanced(m_Box, &projectiles[0], index);
		}
	
	}
	{
		Shader::use(m_ShaderBasic);

		ShaderUniformData* uniformPvm = m_ShaderBasic->m_CommonUniforms.m_ProjectionViewMatrix;
		uniformPvm->setData(a_Camera);
		Shader::applyUniform(uniformPvm);

		ShaderUniformData* uniformColor = m_ShaderBasic->m_CommonUniforms.m_Color;
		glm::vec4 colors[3] = { glm::vec4(1,1,1,1), glm::vec4(0,0,1,1), glm::vec4(0.25f,0.25f,0.25f,1) };

		//uniformColor->setData(&colors[0]);
		//Shader::applyUniform(uniformColor);
		//
		//m_QuadMesh->m_Meshs[0]->setTexture(m_ReflectionBuffer->getTexture());
		//drawObject(m_Ground, false);

		uniformColor->setData(&colors[0]);
		Shader::applyUniform(uniformColor);

		m_Player->draw();

		//Logging::quickGpuDebugGroupPush("TREE RENDER");
		//for (int i = 0; i < NUM_OF_TREES; i++) {
		//	drawObject(m_Trees[i], true);
		//}
		//Logging::quickGpuDebugGroupPop();

		//draw Projectiles
		//uniformColor->setData(&colors[1]);
		//Shader::applyUniform(uniformColor);
		//
		//for (unsigned int i = 0; i < MAX_NUM_OF_PROJECTILES; i++) {
		//	if (m_Projectiles[i] != nullptr) {
		//		//m_Projectiles[i]->draw();
		//		drawObject(m_Projectiles[i]);
		//	}
		//}
		//
		//uniformColor->setData(&colors[0]);
		//Shader::applyUniform(uniformColor);

		//draw enemies
		//for (unsigned int i = 0; i < MAX_NUM_OF_ENEMIES; i++) {
		//	if (m_Enemies[i] != nullptr) {
		//		//m_Projectiles[i]->draw();
		//		drawObject(m_Enemies[i]);
		//	}
		//}
	}
}
