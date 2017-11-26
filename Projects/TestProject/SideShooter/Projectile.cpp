#include "Projectile.h"

#include "TimeHandler.h"
#include "Application.h"

#include "TimeHandler.h"

#include "Helpers.h"
#include "SideShooterContants.h"

Projectile::Projectile(bool a_Direction) {
	m_DirectionScale = a_Direction ? 1 : -1;

	m_StartTime = TimeHandler::getCurrentTime();
	float rand1 = (rand() % 100 / 100.0f)-0.5f * 2.0f;
	float rand2 = rand() % 100 /100.0f;
	m_AliveTime = m_TimeAlive + rand1 * (rand2*m_TimeAliveRandomOffsetMax);
}

Projectile::~Projectile() {}

void Projectile::update() {
	m_Transform.translate(glm::vec3(m_ProjectileSpeed * m_DirectionScale * m_SpeedScale, 0, 0) * TimeHandler::getDeltaTime());

	float percentage = (TimeHandler::getCurrentTime() - m_StartTime) / m_AliveTime;

	if (percentage > m_ScaleDownPercent) {
		float percentage2 = 1 -((percentage - m_ScaleDownPercent) / (1 - m_ScaleDownPercent));
		m_Transform.setScale(percentage2);
		m_SpeedScale = (percentage2 + 0.5f) / 1.5f;
	}
 	if (percentage > 1) {
		m_ShouldDelete = true;
	}

	objectSideWarp(&m_Transform);
}
