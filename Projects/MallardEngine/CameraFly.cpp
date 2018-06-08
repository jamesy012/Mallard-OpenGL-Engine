#include "CameraFly.h"

#include "Input.h"
#include "TimeHandler.h"
#include "Window.h"

CameraFly::CameraFly() {
}


CameraFly::~CameraFly() {
}

void CameraFly::update() {
	//check if we should move the camera?
	bool mouseMovement = true;
	//is the flag for only moving while locked on?
	//if (m_CameraRotationOnlyWhileMouseLocked) {
	//	//set mouse movement bool to if the mouse is locked
	//	mouseMovement = Window::isMouseLocked();
	//}
	//if we should move the mouse
	if (m_RotationMode != 0) {
		//move mouse
		glm::vec2 movement;
		if (m_RotationMode & CameraFlyRoationModes::MOUSE) {
			movement = -glm::vec2(Input::getMouseDeltaX(), Input::getMouseDeltaY());
		} 
		if (m_RotationMode & CameraFlyRoationModes::KEYS) {
			if (Input::isKeyDown(m_KeyboardKeys.RotateLeft)) {
				movement.x += m_KeyRotationSpeed;
			}
			if (Input::isKeyDown(m_KeyboardKeys.RotateRight)) {
				movement.x += -m_KeyRotationSpeed;
			}
			if (Input::isKeyDown(m_KeyboardKeys.RotateUp)) {
				movement.y += m_KeyRotationSpeed;
			}
			if (Input::isKeyDown(m_KeyboardKeys.RotateDown)) {
				movement.y += -m_KeyRotationSpeed;
			}
		}
		if (movement != glm::vec2(0)) {
			m_IsDirty = true;

#ifdef USE_QUATERNIONS
			movement *= 0.0025f;//sensitivity

			//rotate Up and down
			//m_Transform.rotate(glm::quat(glm::rotate(movement.y, glm::vec3(1, 0, 0))));
			glm::quat rot = m_Transform.getLocalRotation();
			rot = glm::rotate(rot, movement.y, glm::vec3(1, 0, 0));
			m_Transform.setRotation(rot);

			//calc up
			glm::vec4 up = glm::inverse(rot) * glm::vec4(0, 1, 0, 0);
			//
			////rotate left and right using the up vector
			m_Transform.setRotation(glm::rotate(rot, movement.x, glm::vec3(up[0], up[1], up[2])));
#else

			//attempt to stop the weird left/right controls when upside down
			glm::vec3 rotation = m_Transform.getLocalRotationEulers();
			int xRot = (int)abs(rotation.x) % 360;
			if (xRot > 90 && xRot < 270) {
				movement.x *= -1;
			}

			//up/down
			m_Transform.rotate(glm::vec3(movement.y, 0, 0));
			//left/right
			m_Transform.rotate(glm::vec3(0, movement.x, 0));
#endif // USE_QUATERNIONS

		}
		//scroll
		//if (m_AllowChangeOfFovByScroll) {
		//	float scrollDelta = -Input::getMouseScrollY();//scroll is backwards for what we want
		//	if (scrollDelta != 0) {
		//		scrollDelta *= 0.1f;//sensitivity
		//		float fov = m_FieldOfView + scrollDelta;
		//		fov = fov < 0.1f ? 0.1f : fov > 2 ? 2 : fov;//clamp
		//		setPerspective(fov, m_Aspect, m_NearPlane, m_FarPlane);
		//	}
		//}
	}

	//bool keyMovement = true;
	//if (m_MovementOnlyWhileMouseLocked) {
	//	keyMovement = Window::isMouseLocked();
	//}
	//if (!keyMovement) {
	//	return;
	//}

	//z will center view to (0,0,0)
	//if (Input::isKeyDown(GLFW_KEY_Z)) {
	//	setLookAt(m_Position, glm::vec3(0));
	//	//return;
	//}

	//how much to move in each direction
	glm::vec3 movement;
	if (Input::isKeyDown(m_KeyboardKeys.MoveForward)) {
		movement.z -= m_Speed;
	}
	if (Input::isKeyDown(m_KeyboardKeys.MoveBackwards)) {
		movement.z += m_Speed;
	}
	if (Input::isKeyDown(m_KeyboardKeys.MoveLeft)) {
		movement.x -= m_Speed;
	}
	if (Input::isKeyDown(m_KeyboardKeys.MoveRight)) {
		movement.x += m_Speed;
	}
	if (Input::isKeyDown(m_KeyboardKeys.MoveUp)) {
		movement.y += m_Speed;
	}
	if (Input::isKeyDown(m_KeyboardKeys.MoveDown)) {
		movement.y -= m_Speed;
	}

	if (movement == glm::vec3(0)) {
		return;
	}

	m_Transform.translate(movement * TimeHandler::getDeltaTime(), false);

}
