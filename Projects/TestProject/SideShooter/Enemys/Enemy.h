#pragma once
#include "Object.h"

class Player;
class SideShooter;

class Enemy : public Object {
public:
	Enemy(IRenderable* a_Model, Player* a_Player, glm::vec3 a_StartingPos, IRenderable* a_ProjectileModel, SideShooter* a_App);
	~Enemy();

	void update() override;

	//direction of the ship
	bool m_Direction;

	//how fast this enemy moves, scaled by deltatime
	float m_MovementSpeed = 1;

	float m_TimeForIntro = 2.0f;
protected:
	virtual void normalUpdate() = 0;

	void loadModel(const char* a_Path);

	//reference to the player
	Player* m_Player;
	//reference to the projectile model
	IRenderable* m_ProjectileModel;
	//reference to the main App
	//we need this to be able to spwan projectiles
	SideShooter* m_App;

	bool m_IsDoingIntro = true;
	//starting point for the enemy;
	glm::vec3 m_StartPos;
	//when was this enemy spawned
	float m_StartTime = 0;
};

