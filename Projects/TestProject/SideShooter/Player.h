#pragma once
#include "Object.h"

class Model;
class Camera;


class Player : public Object {
public:
	Player();
	~Player();

	void update() override;

	//reference to the camera 
	Camera* m_Camera = nullptr;

	//transform that contains the global shoot point for projectiles
	Transform m_ShootPoint;

	//what direction is the player facing
	bool m_FacingRight = true;
private:
	//reference to the model for this Object
	Model* m_PlayerModel = nullptr;

	//offset - unused in current model
	float m_PlayerHeight = 0.0f;
};

