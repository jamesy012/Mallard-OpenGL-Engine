#include "Projectile.h"

#include "TimeHandler.h"
#include "Application.h"

#include "Helpers.h"

Projectile::Projectile(bool a_Direction) {
	m_DirectionScale = a_Direction ? 1 : -1;
}

Projectile::~Projectile() {}

void Projectile::update() {
	m_Transform.translate(glm::vec3(m_ProjectileSpeed * m_DirectionScale, 0, 0) * TimeHandler::getDeltaTime());
	if (!isOnScreen(m_Transform.getGlobalPosition(), m_Camera)) {
		m_ShouldDelete = true;
	}
}
