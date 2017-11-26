#pragma once
#include "Object.h"

class Camera;

class Projectile : public Object {
public:
	Projectile(bool a_Direction);
	~Projectile();

	void update() override;

	bool m_ShouldDelete = false;
	Camera* m_Camera;

	float m_ProjectileSpeed = 100.0f;

	//how long this projectile is alive in seconds
	const float m_TimeAlive = 5.0f;
	const float m_TimeAliveRandomOffsetMax = 2.0f;
private:
	//scale for the direction of the projectile
	//wiill be -1 or 1
	int m_DirectionScale;

	float m_SpeedScale = 1;

	float m_StartTime = 0.0f;
	float m_AliveTime;

	//at what percent will the projectiles start scaling down
	const float m_ScaleDownPercent = 0.7f;
};

