#pragma once
#include "Enemy.h"

class EnemyStationary : public Enemy {
public:
	EnemyStationary(IRenderable* a_Model, Player* a_Player, glm::vec3 a_StartingPos, IRenderable* a_ProjectileModel, SideShooter* a_App);
	~EnemyStationary();

	float const m_MovementScale = 5;
	float m_TimeScale = 1;

	float const m_RotationSpeed = 30.0f;
private:
	//small offset for the start time, to add some variation in the movement
	float m_TimeOffset = 0;

	// Inherited via Enemy
	virtual void normalUpdate() override;
};

