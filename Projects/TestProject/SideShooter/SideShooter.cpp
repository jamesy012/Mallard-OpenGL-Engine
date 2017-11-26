#include "SideShooter.h"

#include <stdio.h>

#include <gl\glew.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "TimeHandler.h"
#include "Window.h"
#include "Input.h"
#include "Keys.h"
#include "Logging.h"

#include "Shader.h"
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

void SideShooter::startUp() {

	m_TestText = new Text(m_Font);
	m_TestText->generateText(R"(Side Shooter
		Move: WASD, Shoot: Space)");		

	m_TreeModel = new Model();
	m_TreeModel->load("Models/SideShooter/lowpolytree.obj");
	//m_TreeModel->m_Transform.setScale(0.05f);

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

	for (int i = 0; i < NUM_OF_TREES; i++) {
		m_Trees[i] = new Object("TREE!");
		m_Trees[i]->m_Renderable = m_TreeModel;
		m_Trees[i]->m_Transform.setPosition(
			glm::vec3(
			getRandomWithinRange(-SSConstants::GAME_WIDTH,SSConstants::GAME_WIDTH),
			-SSConstants::GROUND_Y,
			-getRandomWithinRange(0, 300)-10));
		m_Trees[i]->m_Transform.setScale(getRandomWithinRange(2, 5));
		float xzRot = 5.0f;
		m_Trees[i]->m_Transform.setRotation(
			glm::vec3(
			getRandomWithinRange(-xzRot, xzRot),
			getRandomWithinRange(0, 360),
			getRandomWithinRange(-xzRot, xzRot)));
	}
}

void SideShooter::shutDown() {
	m_TreeModel->unload();
	m_QuadMesh->unload();
	m_GrassTexture->unload();

	delete m_Box;

	delete m_Player;
	delete m_Ground;

	delete m_TestText;

	delete m_Camera;

	for (unsigned int i = 0; i < NUM_OF_PROJECTILES; i++) {
		if (m_Projectiles[i] != nullptr) {
			delete m_Projectiles[i];
		}
	}

	for (int i = 0; i < NUM_OF_TREES; i++) {
		delete m_Trees[i];
	}
}

void SideShooter::update() {
	m_Player->update();

	if (Input::isKeyRepeated(KEY_SPACE)) {
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

	float scale = ((m_Player->m_Movement.x * 1.25f) / m_Player->m_MovementSpeed) * -1;
	m_CameraGame->m_Transform.rotate(glm::vec3(0, scale, 0) * TimeHandler::getDeltaTime());
}

void SideShooter::draw() {
	Shader::use(m_ShaderBasic);

	ShaderUniformData* uniformPvm = m_ShaderBasic->m_CommonUniforms.m_ProjectionViewMatrix;
	uniformPvm->setData(m_Camera);
	Shader::applyUniform(uniformPvm);

	ShaderUniformData* uniformColor = m_ShaderBasic->m_CommonUniforms.m_Color;
	glm::vec4 colors[3] = { glm::vec4(1,1,1,1), glm::vec4(0,0,1,1), glm::vec4(0.25f,0.25f,0.25f,1) };

	m_Player->draw();

	for (int i = 0; i < NUM_OF_TREES; i++) {
		drawObject(m_Trees[i]);
	}

	uniformColor->setData(&colors[1]);
	Shader::applyUniform(uniformColor);

	for (unsigned int i = 0; i < NUM_OF_PROJECTILES; i++) {
		if (m_Projectiles[i] != nullptr) {
			//m_Projectiles[i]->draw();
			drawObject(m_Projectiles[i]);
		}
	}

	uniformColor->setData(&colors[2]);
	Shader::applyUniform(uniformColor);
	drawObject(m_Ground, false);

	uniformColor->setData(&colors[0]);
	Shader::applyUniform(uniformColor);
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
