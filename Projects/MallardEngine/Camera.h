#pragma once
#include "DLLBuild.h"
#include "Object.h"

class DLL_BUILD Camera : public Object {
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

	Camera& operator=(const Camera& a_Copy); // copy assignment

	void setPerspective(const float a_Fov, const float a_Aspect, const float a_Near, const float a_Far);
	void setOrthographic(const float a_Left, const float a_Right, const float a_Bottom, const float a_Top, const float a_Near, const float a_Far);

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getProjectionViewMatrix();

	bool isDirty() const {
		return m_IsDirty;
	}

	// do nothing
	virtual void update() override {}

private:
	//uses the Transform to update the ViewMatrix
	void updateView();

	//updates the view
	//and if something has changed since the last time the pv matrix was made
	//it will create it again
	void updatePV();

	//storage for the perspective/orthographic info that was given to the camera
	CameraData m_Data;

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ProjectionViewMatrix;

	//number relating to the last frame the View matrix was updated
	unsigned int m_LastViewMatrixUpdateFrame = -1;

	//a check to see if this camera is dirty
	//this will be true if the perspective, orthographic or m_Transform was changed
	//and false again after updatePV is called
	bool m_IsDirty = false;



};

