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
private:
	//scale for the direction of the projectile
	//wiill be -1 or 1
	int m_DirectionScale;
};

