#include "Camera.h"

#include <glm\glm.hpp>
//perspective/ortho
#include <glm\ext.hpp>

Camera::Camera() : Object("BaseCamera") {
}


Camera::~Camera() {
}

Camera & Camera::operator=(const Camera & a_Copy) {
	m_Data = a_Copy.m_Data;
	m_ProjectionMatrix = a_Copy.m_ProjectionMatrix;
	m_Transform.setPosition(a_Copy.m_Transform.getLocalPosition());
	m_Transform.setRotation(a_Copy.m_Transform.getLocalRotation());
	m_Transform.setScale(a_Copy.m_Transform.getLocalScale());
	m_IsDirty = true;
	m_LastViewMatrixUpdateFrame = -1;
	return *this;
}

void Camera::setPerspective(const float a_Fov, const float a_Aspect, const float a_Near, const float a_Far) {
	m_Data.m_Perspective.m_Fov = glm::radians(a_Fov);
	m_Data.m_Perspective.m_Aspect = a_Aspect;
	m_Data.m_Near = a_Near;
	m_Data.m_Far = a_Far;
	m_IsDirty = true;

	m_ProjectionMatrix = glm::perspective(m_Data.m_Perspective.m_Fov,
										  m_Data.m_Perspective.m_Aspect,
										  m_Data.m_Near,
										  m_Data.m_Far);
}

void Camera::setOrthographic(const float a_Left, const float a_Right, const float a_Bottom, const float a_Top, const float a_Near, const float a_Far) {
	m_Data.m_Orthographic.m_Left = a_Left;
	m_Data.m_Orthographic.m_Right = a_Right;
	m_Data.m_Orthographic.m_Bottom = a_Bottom;
	m_Data.m_Orthographic.m_Top = a_Top;
	m_Data.m_Near = a_Near;
	m_Data.m_Far = a_Far;

	m_IsDirty = true;

	m_ProjectionMatrix = glm::ortho(m_Data.m_Orthographic.m_Left,
									m_Data.m_Orthographic.m_Right,
									m_Data.m_Orthographic.m_Bottom,
									m_Data.m_Orthographic.m_Top,
									m_Data.m_Near,
									m_Data.m_Far);
}

//runs UpdateView to check if we should update the view matrix by using the transform
//then returns the view matrix
glm::mat4 Camera::getViewMatrix() {
	updateView();
	return m_ViewMatrix;
}

//this doesn't require updating since it's from the setPerspective and setOrthographic functions
glm::mat4 Camera::getProjectionMatrix() {
	return m_ProjectionMatrix;
}

//runs the update on the updatePV function then returns the m_ProjectionViewMatrix
//this also updates the view matrix
glm::mat4 Camera::getProjectionViewMatrix() {
	updatePV();
	return m_ProjectionViewMatrix;
}

//the view matrix is the inverse of the globalMatrix of the transform
void Camera::updateView() {
	//check to see if the transforms last update was different to the last time the camera updated it's view matrix
	//or the transform/parent is dirty
	if (m_Transform.getLastTransformUpdate() != m_LastViewMatrixUpdateFrame || m_LastViewMatrixUpdateFrame == -1 || m_Transform.isParentDirty()) {
		//sets the view matrix to the inverse of the transforms matrix
		m_ViewMatrix = glm::inverse(m_Transform.getGlobalMatrix());

		m_LastViewMatrixUpdateFrame = m_Transform.getLastTransformUpdate();
		m_IsDirty = true;
	}
}

void Camera::updatePV() {
	updateView();
	if (m_IsDirty) {
		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;
		m_IsDirty = false;
	}
}
