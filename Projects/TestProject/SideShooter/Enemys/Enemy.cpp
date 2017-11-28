#include "Enemy.h"

#include "Model.h"
#include "TimeHandler.h"

#include "../Helpers.h"
#include "../SideShooterContants.h"

Enemy::Enemy(IRenderable* a_Model, Player* a_Player, glm::vec3 a_StartingPos, IRenderable* a_ProjectileModel, SideShooter* a_App)  {
	m_Renderable = a_Model;
	m_Player = a_Player;
	m_Transform.setPosition(a_StartingPos);
	m_ProjectileModel = a_ProjectileModel;
	m_StartTime = TimeHandler::getCurrentTime();
	m_StartPos = a_StartingPos;

	m_Transform.setPosition(m_StartPos + glm::vec3(0, SSConstants::GAME_HEIGHT*3, 0));
}


Enemy::~Enemy() {
	if (m_Renderable != nullptr) {
		//convert renderable to model then delete
		Model* model = (Model*)m_Renderable;
		model->unload();
	}
}

void Enemy::update() {
	if (m_IsDoingIntro) {
		glm::vec3 lerpPos = lerp(m_Transform.getLocalPosition(), m_StartPos + glm::vec3(0,-2,0), 0.01);
		if (lerpPos.y < m_StartPos.y) {
			m_IsDoingIntro = false;
			m_StartTime = TimeHandler::getCurrentTime();
		}
		m_Transform.setPosition(lerpPos);
	} else {
		normalUpdate();
	}
}

void Enemy::loadModel(const char * a_Path) {
	Model* model = new Model();
	model->load(a_Path);
	m_Renderable = model;
}
