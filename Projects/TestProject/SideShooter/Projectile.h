#pragma once
#include "Object.h"

class Camera;

class Projectile : public Object {
public:
	Projectile();
	~Projectile();

	void update() override;

	bool m_ShouldDelete = false;
	Camera* m_Camera;

	float m_ProjectileSpeed = 100.0f;
};

