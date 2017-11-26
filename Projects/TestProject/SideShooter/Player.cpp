#include "Player.h"

#include "Input.h"
#include "Keys.h"
#include "Window.h"
#include "TimeHandler.h"

#include "Model.h"

#include "SideShooterContants.h"
#include "Helpers.h"

Player::Player() : Object("Player") {
	m_PlayerModel = new Model();
	//m_PlayerModel->load("Models/Nanosuit/nanosuit.obj");
	m_PlayerModel->load("Models/SideShooter/ship.obj");
	m_PlayerModel->m_Transform.setScale(2);
	m_Renderable = m_PlayerModel;

	//move the player to the left by 45
	m_Transform.setPosition(glm::vec3(-45.0f, 0.0f, 0.0f));

	//set up the shootPoint
	m_ShootPoint.setParent(&m_Transform);
	m_ShootPoint.setPosition(glm::vec3(2, 0, 0));
}


Player::~Player() {
	m_PlayerModel->unload();
}

void Player::update() {

	float screenSizeOffsets = Window::getMainWindow()->getWindowHeight()*0.1f;

	float horizontalMoveSpeed = m_MovementSpeed * m_DirectionFacing;

	bool isMoving = false;
	m_Movement = glm::vec3(0);

	int movement = 0;
	movement |= (Input::isKeyDown(KEY_D) || Input::isKeyDown(KEY_RIGHT)) << 0;
	movement |= (Input::isKeyDown(KEY_A) || Input::isKeyDown(KEY_LEFT)) << 1;
	if (movement == 1) {
		m_Movement.x += horizontalMoveSpeed * TimeHandler::getDeltaTime();
		isMoving = true;
		m_Transform.setRotation(glm::vec3(0, 0, 0));
		if (!m_FacingRight) {
			swapDirection();
		}
	} else if (movement == 2) {
		m_Movement.x += horizontalMoveSpeed * TimeHandler::getDeltaTime();
		isMoving = true;
		m_Transform.setRotation(glm::vec3(0, 180, 0));
		if (m_FacingRight) {
			swapDirection();
		}
	} else {

	}

	if (Input::isKeyDown(KEY_W) || Input::isKeyDown(KEY_UP)) {
		m_Movement.y += m_MovementSpeed * TimeHandler::getDeltaTime();
		//glm::vec2 screenPos = worldToScreenSpace(m_Transform.getLocalPosition() + moveAmount + glm::vec3(0, m_PlayerHeight, 0), m_Camera);
		//if (screenPos.y < Window::getMainWindow()->getWindowHeight() - screenSizeOffsets) {
		if ((m_Transform.getLocalPosition() + m_Movement).y < SSConstants::GAME_HEIGHT - m_PlayerHeight) {
			isMoving = true;
		} else {
			m_Movement.y = 0;
		}
	}
	if (Input::isKeyDown(KEY_S) || Input::isKeyDown(KEY_DOWN)) {
		m_Movement.y += -m_MovementSpeed * TimeHandler::getDeltaTime();
		//glm::vec2 screenPos = worldToScreenSpace(m_Transform.getLocalPosition() - glm::vec3(0, m_PlayerHeight, 0), m_Camera);
		//if (screenPos.y > screenSizeOffsets) {
		if ((m_Transform.getLocalPosition() + m_Movement).y > -SSConstants::GAME_HEIGHT + m_PlayerHeight) {
			isMoving = true;
		} else {
			m_Movement.y = 0;
		}
	}


	if (isMoving) {
		m_Transform.translate(m_Movement);
		m_Movement /= TimeHandler::getDeltaTime();
	}


	objectSideWarp(&m_Transform);

}

void Player::swapDirection() {
	m_IsChangingDirection = true;
	m_FacingRight = !m_FacingRight;
	m_SwapStartTime = TimeHandler::getCurrentTime();
	m_DirectionFacing = m_FacingRight ? 1 : -1;
}
