#include "Player.h"

#include "Input.h"
#include "Keys.h"
#include "Window.h"
#include "TimeHandler.h"

#include "Model.h"

#include "Helpers.h"

Player::Player() : Object("Player") {
	m_PlayerModel = new Model();
	//m_PlayerModel->load("Models/Nanosuit/nanosuit.obj");
	m_PlayerModel->load("Models/SideShooter/ship.obj");
	m_PlayerModel->m_Transform.setScale(2);
	m_Renderable = m_PlayerModel;
	
	//move the model up by 7 units
	m_PlayerModel->m_Transform.setPosition(glm::vec3(0, -m_PlayerHeight, 0));

	//move the player to the left by 45
	m_Transform.setPosition(glm::vec3(-45.0f, 0.0f, 0.0f));

	//set up the shootPoint
	m_ShootPoint.setParent(&m_Transform);
	m_ShootPoint.setPosition(glm::vec3(2, -2, 0));
}


Player::~Player() {
	m_PlayerModel->unload();
}

void Player::update() {
	float cameraMoveSpeed = 50.0f;
	float screenSizeOffsets = Window::getMainWindow()->getWindowHeight()*0.1f;

	bool isMoving = false;
	glm::vec3 moveAmount;


	if (Input::isKeyDown(KEY_D)) {
		moveAmount.x += cameraMoveSpeed * TimeHandler::getDeltaTime();
		isMoving = true;
		m_Transform.setRotation(glm::vec3(0, 0, 0));
		m_FacingRight = true;
	}
	if (Input::isKeyDown(KEY_A)) {
		moveAmount.x += -cameraMoveSpeed * TimeHandler::getDeltaTime();
		isMoving = true;
		m_Transform.setRotation(glm::vec3(0, 180, 0));
		m_FacingRight = false;
	}

	if (Input::isKeyDown(KEY_W)) {
		moveAmount.y += cameraMoveSpeed * TimeHandler::getDeltaTime();
		glm::vec2 screenPos = worldToScreenSpace(m_Transform.getLocalPosition() + moveAmount + glm::vec3(0, m_PlayerHeight, 0), m_Camera);
		if (screenPos.y < Window::getMainWindow()->getWindowHeight() - screenSizeOffsets) {
			isMoving = true;
		} else {
			moveAmount.y = 0;
		}
	}
	if (Input::isKeyDown(KEY_S)) {
		moveAmount.y += -cameraMoveSpeed * TimeHandler::getDeltaTime();
		glm::vec2 screenPos = worldToScreenSpace(m_Transform.getLocalPosition() - glm::vec3(0, m_PlayerHeight, 0), m_Camera);
		if (screenPos.y > screenSizeOffsets) {
			isMoving = true;
		} else {
			moveAmount.y = 0;
		}
	}


	if (isMoving) {
		m_Transform.translate(moveAmount);
	}


}
