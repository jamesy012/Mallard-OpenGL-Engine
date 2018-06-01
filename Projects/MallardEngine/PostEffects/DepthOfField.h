#pragma once
#include "DLLBuild.h"

class Framebuffer;
class Shader;

class RenderMList;
class Camera;

class Mesh;

class DLL_BUILD DepthOfField {
public:
	DepthOfField();
	~DepthOfField();

	void create();

	void use(Camera* a_Camera, RenderMList* a_List);

	float m_ScaledSize = 3;
	unsigned int m_Width = 1024;
	unsigned int m_Height = 1024;

	Framebuffer* m_ReadBuffer;

//private:
	//Depth of field
	Framebuffer * m_DOF = nullptr;
	Framebuffer* m_DOFScaled = nullptr;
	Framebuffer* m_DOFScaledBlurred = nullptr;

	Shader* m_DOFGenShader = nullptr;
	Shader* m_DOFDrawShader = nullptr;

	Shader* m_BlurShader;

	Mesh* m_FullScreenQuad;
};

