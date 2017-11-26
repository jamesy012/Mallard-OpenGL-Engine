#include "SideShooterCamera.h"

#include "Player.h"
#include "Helpers.h"
#include "SideShooterContants.h"

SideShooterCamera::SideShooterCamera(Player* a_Player) : m_Player(a_Player) {
}

SideShooterCamera::~SideShooterCamera() {
}

void SideShooterCamera::update() {
	//player side warp
	{
		float xOffset = m_Player->m_Transform.getGlobalPosition().x - m_Transform.getGlobalPosition().x;
		if (abs(xOffset) > SSConstants::GAME_WIDTH) {
			if (xOffset > 0) {
				m_Transform.translate(glm::vec3(SSConstants::GAME_WIDTH * 2, 0, 0));
			} else {
				m_Transform.translate(glm::vec3(-SSConstants::GAME_WIDTH * 2, 0, 0));
			}
		}
	}

	glm::vec2 movement = m_Player->m_Movement / m_Player->m_MovementSpeed;

	//position
	{
		glm::vec3 targetPos = m_Player->m_Transform.getGlobalPosition() + (m_Player->m_Renderable->m_Transform.getLocalPosition()*glm::vec3(2));
		targetPos.x += m_Player->m_DirectionFacing * m_XFacingOffset;
		targetPos.x += movement.x * m_XFacingMovingOffset;
		targetPos.y *= 0.3f;
		targetPos += m_Offset;

		targetPos.y += abs(movement.x) * 2;

		//m_Transform.setPosition(targetPos + m_Offset);
		glm::vec3 newPos = lerp(m_Transform.getGlobalPosition(), targetPos, 0.1);
		m_Transform.setPosition(newPos);
	}

	//rotation
	{
		float rotTargetZ = movement.x * -4;
		float rotTargetX = movement.y * 10;

		m_RotCurrentZ = lerp(m_RotCurrentZ, rotTargetZ, m_RotSpeedZ);
		m_RotCurrentX = lerp(m_RotCurrentX, rotTargetX, m_RotSpeedX);

		glm::vec3 rot;
		rot.z = m_RotCurrentZ;
		rot.x = m_RotCurrentX;
		m_Transform.setRotation(rot);
	}
}
