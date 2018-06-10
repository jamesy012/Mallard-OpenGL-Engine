#pragma once
#include "DLLBuild.h"
#include "Camera.h"
#include "Keys.h"




class DLL_BUILD CameraFly : public Camera {
public:
	const struct RotationModes {
		static const char OFF = 0;
		static const char MOUSE = 1;
		static const char KEYS = 2;
	};
	typedef char RotationModesValue;
public:
	CameraFly();
	~CameraFly();

	virtual void update() override;

	float m_MovementSpeed = 10.0f;
	float m_KeyRotationSpeed = 50.0f;
	float m_MouseRotationSpeed = 1000.0f;

	//use values from CameraFly::RotationModes when setting
	RotationModesValue m_RotationMode = RotationModes::KEYS;
	//flag to turn on or off moving the camera via keys
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

