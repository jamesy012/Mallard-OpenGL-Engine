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

static bool objectDistSort(glm::mat4& a, glm::mat4& b) {
	return glm::value_ptr(a)[14] < glm::value_ptr(b)[14];
}

void SideShooter::startUp() {

	m_TestText = new Text(m_Font);
	m_TestText->generateText(R"(Side Shooter
		Move: WASD, Shoot: Space)");		

	m_TreeModel = new Model();
	m_TreeModel->load("Models/SideShooter/lowpolytree.obj");
	//m_TreeModel->m_Transform.setScale(0.05f);

	m_UniformShader = new Shader();
	m_UniformShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/BasicInstancing.vert");
	m_UniformShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/BasicInstancing.frag");
	m_UniformShader->linkShader();

	m_ReflectionShader = new Shader();
	m_ReflectionShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/Reflection.vert");
	m_ReflectionShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/Reflection.frag");
	m_ReflectionShader->linkShader();

	m_Box = new Mesh();
	m_Box->createBox();

	m_GrassTexture = new Texture();
	m_GrassTexture->load("Models/SideShooter/Grass.jpg");

	m_QuadMesh = new Model();
	m_QuadMesh->load("Models/SideShooter/UvScaledPlane.obj");
	m_QuadMesh->m_Meshs[0]->setTexture(m_GrassTexture);

	m_Ground = new Object("Ground Plane");
	m_Ground->m_Renderable = m_QuadMesh;
	m_Ground->m_Transform.setPosition(glm::vec3(0, -SSConstants::GROUND_Y, -150));
	m_Ground->m_Transform.setScale(2);

	m_Player = new Player();
	m_Camera = new SideShooterCamera(m_Player);

	m_Camera->setPerspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	m_Player->m_Camera = m_Camera;

	m_ReflectionCamera = new Camera();
	m_ReflectionCamera->setPerspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

	m_ReflectionBuffer = new Framebuffer();
	m_ReflectionBuffer->setSize(Window::getMainWindow()->getFramebufferWidth(), Window::getMainWindow()->getFramebufferHeight());
	m_ReflectionBuffer->createRenderTarget();	

	for (int i = 0; i < NUM_OF_TREES; i++) {
		Transform t;
		t.setPosition(
			glm::vec3(
			getRandomWithinRange(-SSConstants::GAME_WIDTH,SSConstants::GAME_WIDTH),
			-SSConstants::GROUND_Y,
			-getRandomWithinRange(0, 300)-10));
		t.setScale(getRandomWithinRange(2, 5));
		float xzRot = 5.0f;
		t.setRotation(
			glm::vec3(
			getRandomWithinRange(-xzRot, xzRot),
			getRandomWithinRange(0, 360),
			getRandomWithinRange(-xzRot, xzRot)));
		m_UniformTrees[i] = t.getGlobalMatrix();
		float test = glm::value_ptr(m_UniformTrees[i])[14];
	}
	//std::sort(std::begin(m_UniformTrees), std::end(m_UniformTrees), objectDistSort);
}

void SideShooter::shutDown() {
	m_TreeModel->unload();
	m_QuadMesh->unload();
	m_GrassTexture->unload();

	delete m_UniformShader;

	delete m_Box;

	delete m_Player;
	delete m_Ground;

	delete m_TestText;

	delete m_Camera;

	delete m_ReflectionBuffer;
	delete m_ReflectionCamera;
	delete m_ReflectionShader;

	for (unsigned int i = 0; i < NUM_OF_PROJECTILES; i++) {
		if (m_Projectiles[i] != nullptr) {
			delete m_Projectiles[i];
		}
	}
}

void SideShooter::update() {
	if (Input::wasKeyPressed(KEY_Q)) {
		m_ReflectionShader->reloadShaders();
	}

	m_Player->update();

	if (Input::wasKeyPressed(KEY_SPACE)) {
		//find next empty spot
		for (unsigned int i = 0; i < NUM_OF_PROJECTILES; i++) {
			if (m_Projectiles[i] == nullptr) {
				m_Projectiles[i] = new Projectile(m_Player->m_DirectionFacing > 0);
				m_Projectiles[i]->m_Renderable = m_Box;
				m_Projectiles[i]->m_Camera = m_Camera;
				m_Projectiles[i]->m_Transform.setPosition(m_Player->m_ShootPoint.getGlobalPosition());
				break;
			}
		}
	}

	for (unsigned int i = 0; i < NUM_OF_PROJECTILES; i++) {
		if (m_Projectiles[i] != nullptr) {
			m_Projectiles[i]->update();
			if (m_Projectiles[i]->m_ShouldDelete) {
				delete m_Projectiles[i];
				m_Projectiles[i] = nullptr;
			}
		}
	}

	m_Camera->update();

	float scale = ((m_Player->m_Movement.x * 1.25f) / m_Player->m_MovementSpeed) * -10;
	m_CameraGame->m_Transform.rotate(glm::vec3(0, scale, 0) * TimeHandler::getDeltaTime());

	glm::vec3 camPos = m_Camera->m_Transform.getGlobalPosition();
	float camY = camPos.y;
	camPos.y = -SSConstants::GROUND_Y -(camPos.y+SSConstants::GROUND_Y);
	glm::vec3 camRot = m_Camera->m_Transform.getGlobalRotationEulers();
	camRot *=  glm::vec3(-1,1,-1);
	m_ReflectionCamera->m_Transform.setPosition(camPos);
	m_ReflectionCamera->m_Transform.setRotation(camRot);
	m_ReflectionCamera->m_Transform.setScale(glm::vec3(1, -1, 1));

	//m_CameraGame->m_Transform.setPosition(glm::vec3(0, m_Camera->m_Transform.getLocalPosition().y, 0));

}

void SideShooter::draw() {
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

	sceneRender(m_ReflectionCamera);

	glEnable(GL_CULL_FACE);

	Framebuffer::use(nullptr);

	sceneRender(m_Camera);

	//render plane
	{
		Shader::use(m_ReflectionShader);

		ShaderUniformData* uniformPvm = m_ReflectionShader->m_CommonUniforms.m_ProjectionViewMatrix;
		uniformPvm->setData(m_Camera);
		Shader::applyUniform(uniformPvm);

		m_QuadMesh->m_Meshs[0]->setTexture(m_ReflectionBuffer->getTexture());
		drawObject(m_Ground, false);
	}
}

void SideShooter::drawUi() {
	//dont run UI atm
	//return;
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

void SideShooter::drawObjectInstanced(IRenderable * a_Renderable, glm::mat4 * a_Array, int a_ArraySize) {
	ShaderUniformData* uniformOffset = m_UniformShader->getUniform(ShaderUniformTypes::VEC4, "offset");
	int loc = glGetUniformLocation(Shader::getCurrentShader()->getProgram(), "models[0]");

	glm::vec4 offset = glm::vec4(0);
	const int MAX_INSTANCES_PER_DRAW = 128;
	float batches = a_ArraySize / (float)MAX_INSTANCES_PER_DRAW;
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

void SideShooter::drawObject(IRenderable* a_Renderable, bool a_Cull) {

	glm::vec3 startingPos = a_Renderable->m_Transform.getGlobalPosition();

	for (int i = -1; i < 2; i++) {
		glm::vec3 newPos = startingPos + glm::vec3(SSConstants::GAME_WIDTH * 2 * i, 0, 0);

		if(a_Cull) {
			float dist = abs(newPos.x - m_Player->m_Transform.getLocalPosition().x) + newPos.z;
			if (dist > SSConstants::CLOSE_RENDER) {
				continue;
			}
		}

		a_Renderable->m_Transform.setPosition(newPos);
		a_Renderable->draw();
	}
	a_Renderable->m_Transform.setPosition(startingPos);

}

void SideShooter::sceneRender(Camera * a_Camera) {
	{
		Shader::use(m_UniformShader);

		ShaderUniformData* uniformPvm = m_UniformShader->m_CommonUniforms.m_ProjectionViewMatrix;
		uniformPvm->setData(a_Camera);
		Shader::applyUniform(uniformPvm);
		Logging::quickGpuDebugGroupPush("TREE RENDER1");

		drawObjectInstanced(m_TreeModel, &m_UniformTrees[0], NUM_OF_TREES);

		Logging::quickGpuDebugGroupPop();
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


		uniformColor->setData(&colors[1]);
		Shader::applyUniform(uniformColor);

		for (unsigned int i = 0; i < NUM_OF_PROJECTILES; i++) {
			if (m_Projectiles[i] != nullptr) {
				//m_Projectiles[i]->draw();
				drawObject(m_Projectiles[i]);
			}
		}
	}
}
