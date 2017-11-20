#pragma once

#include "Application.h"

class Model;
class Mesh;
class Shader;
class Texture;
class Font;
class Text;
class Framebuffer;
class Transform;

class TestApp :
	public Application {
public:
	// Inherited via Application
	virtual void startUp() override;
	virtual void shutDown() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void drawUi() override;

private:
	void updateModel();

	//TODO: Move to another class!
	glm::vec4 frustumPlanes[6];
	int m_LastFrustumUpdate = -1;
	void getFrustumPlanes(const glm::mat4& a_Transform);
	bool checkFustumPlanes(const glm::vec3 a_Position);
	bool m_FirstModelUpdate = true;

	Model* m_Model;
	Mesh* m_GroundPlane;

	Shader* m_TextShader;
	Font* m_Font;
	Text* m_TestText;

	Texture* m_1x1WhiteTexture;

	Framebuffer* m_DirectionalLightFb;

	Shader* m_ShadowGenerationShader;
	Shader* m_ShadowDrawShader;

	glm::vec3 m_LightDir;
	glm::mat4 m_LightMatrix;
	float m_CurrentOrthoSize = 40;

	float m_Spacing = 10;
	int m_AmountPerSide = 5u;

	bool m_UseCulling = false;

	int m_AmountRendering = 0;
	int m_NumberOfFrustumChecks = 0;

	unsigned int m_InstanceArrayBuffer = 0;
};

