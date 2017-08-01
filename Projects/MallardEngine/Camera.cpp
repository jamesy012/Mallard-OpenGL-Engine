#include "Camera.h"

//perspective/ortho
#include <glm\ext.hpp>

Camera::Camera() {
}


Camera::~Camera() {
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

glm::mat4 Camera::getViewMatrix() {
	updateView();
	return m_ViewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() {
	return m_ProjectionMatrix;
}

glm::mat4 Camera::getProjectionViewMatrix() {
	updatePV();
	return m_ProjectionViewMatrix;
}

void Camera::updateView() {
	if (m_Transform.getLastTransformUpdate() != m_LastViewMatrixUpdateFrame || m_Transform.isParentDirty()) {
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
