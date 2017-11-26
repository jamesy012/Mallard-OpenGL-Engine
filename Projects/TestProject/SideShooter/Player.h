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

	//1 = right, -1 = left, can be any number between -1 and 1
	float m_DirectionFacing = 1;

	//how the player is moving, not scaled by deltaTime
	glm::vec3 m_Movement;

	float m_MovementSpeed = 50.0f;
private:
	void swapDirection();

	//when did we start the direction change
	float m_SwapStartTime = 0;
	//how long does it take to change direction
	float m_SwapDirDuration = 2.0f;
	//are we changing direction?
	bool m_IsChangingDirection = false;
	//what direction is the player facing
	bool m_FacingRight = true;

	//reference to the model for this Object
	Model* m_PlayerModel = nullptr;

	//offset - unused in current model
	float m_PlayerHeight = 1.0f;
};

