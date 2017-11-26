#pragma once

#include "Camera.h"

class Player;

class SideShooterCamera : public Camera {
public:
	SideShooterCamera(Player* a_Player);
	~SideShooterCamera();

	void update() override;
private:
	Player* m_Player;
	const glm::vec3 m_Offset = glm::vec3(0, -5, 50);
	const float m_XFacingOffset = 8.0f;
	const float m_XFacingMovingOffset = 10.0f;

	float m_RotCurrentZ = 0, m_RotCurrentX = 0;
	//what mix of RotCurrent_ and the target rotation do we take in the lerp
	const float m_RotSpeedZ = 0.15f, m_RotSpeedX = 0.07f;
};

