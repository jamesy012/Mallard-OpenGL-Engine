#include "EnemyStationary.h"

#include "TimeHandler.h"

#include "..\Player.h"
#include "..\Helpers.h"

EnemyStationary::EnemyStationary(IRenderable* a_Model, Player* a_Player, glm::vec3 a_StartingPos, IRenderable* a_ProjectileModel, SideShooter* a_App)
	: Enemy(a_Model, a_Player, a_StartingPos, a_ProjectileModel, a_App) {
	//m_TimeScale = getRandomWithinRange(1.0f, 10.0f);
	m_TimeOffset = getRandomWithinRange(-0.2f, 0.2f);

	loadModel("Models\\SideShooter\\Enemy1.obj");
}

EnemyStationary::~EnemyStationary() {
}

void EnemyStationary::normalUpdate() {
	m_Renderable->m_Transform.setPosition(glm::vec3(
		0,
		sin((TimeHandler::getCurrentTime() - m_StartTime + m_TimeOffset) * m_TimeScale) * m_MovementScale,
		0));

	m_Renderable->m_Transform.rotate(glm::vec3(
		0,
		m_RotationSpeed * TimeHandler::getDeltaTime(),
		0));
}

