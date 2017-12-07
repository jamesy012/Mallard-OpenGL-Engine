#pragma once

#include "Enemy.h"

class  EnemySider : public Enemy {
public:
	EnemySider(IRenderable* a_Model, Player* a_Player, glm::vec3 a_StartingPos, IRenderable* a_ProjectileModel, SideShooter* a_App);
	~EnemySider();

	//what side should we try to get to
	bool m_RightSide = true;

	float const m_MovementScale = 3;
	float m_TimeScale = 1;

	float const m_RotationSpeed = 30.0f;
private:
	//small offset for the start time, to add some variation in the movement
	float m_TimeOffset = 0;

	// Inherited via Enemy
	virtual void normalUpdate() override;
};

