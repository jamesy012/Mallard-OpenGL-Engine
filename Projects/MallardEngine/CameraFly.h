#pragma once
#include "DLLBuild.h"
#include "Camera.h"
#include "Keys.h"


const struct CameraFlyRoationModes {
	static const char OFF = 0;
	static const char MOUSE = 1;
	static const char KEYS = 2;
};

class DLL_BUILD CameraFly : public Camera {
public:
	CameraFly();
	~CameraFly();

	virtual void update() override;

	float m_Speed = 10.0f;
	float m_KeyRotationSpeed = 10.0f;
	char m_RotationMode = CameraFlyRoationModes::KEYS;
	bool m_AllowMovement = true;

	struct CameraKeys {
	public:
		int RotateLeft = KEY_LEFT;
		int RotateRight = KEY_RIGHT;
		int RotateUp = KEY_UP;
		int RotateDown = KEY_DOWN;

		int MoveForward = KEY_W;
		int MoveBackwards = KEY_S;
		int MoveLeft= KEY_A;
		int MoveRight= KEY_D;
		int MoveUp = KEY_SPACE;
		int MoveDown = KEY_LEFT_CONTROL;
	} m_KeyboardKeys;


};

