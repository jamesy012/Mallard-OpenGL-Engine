#pragma once
#include "DLLBuild.h"

#include <glm\glm.hpp>
#include "Transform.h"

class DLL_BUILD Camera {
private:
	struct CameraData {
		union {
			float m_Data[4];

			struct {
				float m_Fov;
				float m_Aspect;
			} m_Perspective;

			struct {
				float m_Left;
				float m_Right;
				float m_Bottom;
				float m_Top;
			} m_Orthographic;
		};
		float m_Near;
		float m_Far;
	};
public:
	Camera();
	~Camera();

	void setPerspective(const float a_Fov, const float a_Aspect, const float a_Near, const float a_Far);
	void setOrthographic(const float a_Left, const float a_Right, const float a_Bottom, const float a_Top, const float a_Near, const float a_Far);

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getProjectionViewMatrix();

	Transform m_Transform;
private:
	//uses the Transform to update the ViewMatrix
	void updateView();

	void updatePV();

	CameraData m_Data;

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ProjectionViewMatrix;

	unsigned int m_LastViewMatrixUpdateFrame = 0;
	bool m_IsDirty = false;

};

