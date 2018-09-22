#include "CameraFly.h"

#include "Input.h"
#include "TimeHandler.h"
#include "Window.h"
#include "Application.h"

CameraFly::CameraFly() {
}


CameraFly::~CameraFly() {
}

void CameraFly::update() {
	//is the flag for only moving while locked on?
	//if (m_CameraRotationOnlyWhileMouseLocked) {
	//	//set mouse movement bool to if the mouse is locked
	//	mouseMovement = Window::isMouseLocked();
	//}
	
	if (!Window::getMainWindow()->HasMouseLock()) {
		return;
	}

	//if one of the rotation modes are turned on 
	if (m_RotationMode != RotationModes::OFF) {
		//rotate via mouse
		glm::vec2 movement;
		if (m_RotationMode & RotationModes::MOUSE) {
			//might need to scale speed based on screen size!
			movement = -glm::vec2(Input::getMouseDeltaX(), Input::getMouseDeltaY());
			movement /= glm::vec2(Window::getMainWindow()->getFramebufferWidth(), Window::getMainWindow()->getFramebufferHeight());
			movement *= m_MouseRotationSpeed;
		}
		if (m_RotationMode & RotationModes::KEYS) {
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
			//check to see if there was any movement
			if (movement != glm::vec2(0)) {
				//scale by deltaTime
				movement *= TimeHandler::getDeltaTime();
#ifdef USE_QUATERNIONS
				//Not needed???
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

	//check to see if the window is focused, and if we should only listen to movment input if it is focused
	//if (m_MovementOnlyWhileMouseLocked) {
	//	keyMovement = Window::isMouseLocked();
	//}
	
	if (m_AllowMovement) {

		//how much to move in each direction
		glm::vec3 movement;
		if (Input::isKeyDown(m_KeyboardKeys.MoveForward)) {
			movement.z -= m_MovementSpeed;
		}
		if (Input::isKeyDown(m_KeyboardKeys.MoveBackwards)) {
			movement.z += m_MovementSpeed;
		}
		if (Input::isKeyDown(m_KeyboardKeys.MoveLeft)) {
			movement.x -= m_MovementSpeed;
		}
		if (Input::isKeyDown(m_KeyboardKeys.MoveRight)) {
			movement.x += m_MovementSpeed;
		}
		if (Input::isKeyDown(m_KeyboardKeys.MoveUp)) {
			movement.y += m_MovementSpeed;
		}
		if (Input::isKeyDown(m_KeyboardKeys.MoveDown)) {
			movement.y -= m_MovementSpeed;
		}

		//limit speed when holding both forward and right to not be quicker than normal
		//not necessary but could be helpful in the end
		//float length = movement.length();
		//if (length > m_MovementSpeed) {
		//	m_MovementSpeed = m_MovementSpeed * (m_MovementSpeed/length);
		//}

		//check to see if there was any movement
		if (movement == glm::vec3(0)) {
			return;
		}

		m_Transform.translate(movement * TimeHandler::getDeltaTime(), false);
	}

}
