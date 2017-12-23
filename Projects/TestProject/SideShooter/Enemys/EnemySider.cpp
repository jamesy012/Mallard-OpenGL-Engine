#include "EnemySider.h"

#include "TimeHandler.h"

#include "..\Player.h"
#include "..\Helpers.h"

EnemySider::EnemySider(IRenderable* a_Model, Player* a_Player, glm::vec3 a_StartingPos, IRenderable* a_ProjectileModel, SideShooter* a_App)
	: Enemy(a_Model, a_Player, a_StartingPos, a_ProjectileModel, a_App) {
	//m_TimeScale = getRandomWithinRange(1.0f, 10.0f);
	m_TimeOffset = getRandomWithinRange(-0.3f, 0.3f);

	loadModel("Models\\SideShooter\\Enemy2.obj");

	m_Renderable->m_Transform.rotate(glm::vec3(
		0,
		getRandomWithinRange(0,360),
		0));

}

EnemySider::~EnemySider() {
}

void EnemySider::normalUpdate() {
	//player side warp
	{
		float xOffset = m_Player->m_Transform.getGlobalPosition().x - m_Transform.getGlobalPosition().x;
		if (abs(xOffset) > SSConstants::GAME_WIDTH) {
			if (xOffset > 0) {
				m_Transform.translate(glm::vec3(SSConstants::GAME_WIDTH * 2, 0, 0));
			}
			else {
				m_Transform.translate(glm::vec3(-SSConstants::GAME_WIDTH * 2, 0, 0));
			}
		}
	}

	glm::vec3 offset;
	offset.x = (m_RightSide ? 1 : -1) * 40.0f;
	glm::vec3 pos;
	pos = lerp(m_Transform.getLocalPosition(), m_Player->getPositionCombined() + offset, 3 * TimeHandler::getDeltaTime());
	m_Transform.setPosition(pos);
}

