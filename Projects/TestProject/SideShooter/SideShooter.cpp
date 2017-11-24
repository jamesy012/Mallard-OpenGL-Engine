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
#include "Camera.h"

#include "Helpers.h"
#include "Projectile.h"
#include "Player.h"

void SideShooter::startUp() {

	m_TestText = new Text(m_Font);
	m_TestText->generateText(R"(Side Shooter
		Move: WASDQE)");	
	

	//m_PlayerModel->load("Models/toilet/Toilet.obj");

	m_TreeModel = new Model();
	//m_Model->load("Models/Nanosuit/nanosuit.obj");
	m_TreeModel->load("Models/test/lowpolytree.obj");
	m_TreeModel->m_Transform.setScale(0.05f);
	m_TreeModel->m_Transform.setPosition(glm::vec3(0, 0, -10));

	m_Box = new Mesh();
	m_Box->createBox();

	m_QuadMesh = new Mesh();
	m_QuadMesh->createPlane(true);
	//m_QuadMesh->setTexture(m_1x1Texture);

	m_Standard = new Object("Standard");
	m_Standard->m_Renderable = m_TreeModel;

	m_Ground = new Object("Ground Plane");
	m_Ground->m_Renderable = m_QuadMesh;
	m_Ground->m_Transform.setPosition(glm::vec3(0, -27, 0));
	m_Ground->m_Transform.setScale(400);

	m_Camera = new Camera();
	m_Camera->setPerspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	m_Camera->m_Transform.setPosition(glm::vec3(0, 0, 50));

	m_Player = new Player();
	m_Player->m_Camera = m_Camera;
}

void SideShooter::shutDown() {
	m_TreeModel->unload();

	delete m_Box;
	delete m_QuadMesh;

	delete m_Player;
	delete m_Standard;
	delete m_Ground;

	delete m_TestText;

	delete m_Camera;

	for (unsigned int i = 0; i < NUM_OF_PROJECTILES; i++) {
		if (m_Projectiles[i] != nullptr) {
			delete m_Projectiles[i];
		}
	}
}

void SideShooter::update() {
	m_Player->update();

	if (Input::isKeyRepeated(KEY_SPACE)) {
		//find next empty spot
		for (unsigned int i = 0; i < NUM_OF_PROJECTILES; i++) {
			if (m_Projectiles[i] == nullptr) {
				m_Projectiles[i] = new Projectile(m_Player->m_FacingRight);
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

	m_CameraGame->m_Transform.rotate(glm::vec3(0, -10, 0) * TimeHandler::getDeltaTime());
}

void SideShooter::draw() {
	Shader::use(m_ShaderBasic);

	ShaderUniformData* uniformPvm = m_ShaderBasic->m_CommonUniforms.m_ProjectionViewMatrix;
	uniformPvm->setData(m_Camera);
	Shader::applyUniform(uniformPvm);

	ShaderUniformData* uniformColor = m_ShaderBasic->m_CommonUniforms.m_Color;
	glm::vec4 colors[3] = { glm::vec4(1,1,1,1), glm::vec4(0,0,1,1), glm::vec4(0.25f,0.25f,0.25f,1) };

	m_Player->draw();
	m_Standard->draw();

	uniformColor->setData(&colors[1]);
	Shader::applyUniform(uniformColor);

	for (unsigned int i = 0; i < NUM_OF_PROJECTILES; i++) {
		if (m_Projectiles[i] != nullptr) {
			m_Projectiles[i]->draw();
		}
	}

	uniformColor->setData(&colors[2]);
	Shader::applyUniform(uniformColor);
	m_Ground->draw();

	uniformColor->setData(&colors[0]);
	Shader::applyUniform(uniformColor);
}

void SideShooter::drawUi() {
	//dont run UI atm
	return;
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
