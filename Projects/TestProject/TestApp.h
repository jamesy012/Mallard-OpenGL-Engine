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
	//does a frustum check for the shadow light matrix
	void updateShadowModel(unsigned int a_ShadowCascade);

	//TODO: Move to another class!
	void getFrustumPlanes(const glm::mat4& a_Transform, glm::vec4* a_Frustums);
	bool checkFustumPlanes(const glm::vec3 a_Position, glm::vec4* a_Frustums);
	glm::vec4 m_FrustumPlanes[6];
	int m_LastFrustumUpdate = -1;
	bool m_FirstModelUpdate = true;

	static const unsigned int NUM_OF_SHADOW_CASCADES = 3;

	Model* m_Model;
	Model* m_ModelShadow[NUM_OF_SHADOW_CASCADES];
	Mesh* m_GroundPlane;

	Shader* m_TextShader;
	Font* m_Font;
	Text* m_TestText;

	Texture* m_1x1WhiteTexture;

	Framebuffer* m_DirectionalLightFb[NUM_OF_SHADOW_CASCADES];

	Shader* m_ShadowGenerationShader;
	Shader* m_ShadowDrawShader;

	glm::vec3 m_LightDir;
	glm::mat4 m_LightMatrix[NUM_OF_SHADOW_CASCADES];
	float m_CurrentOrthoSize = 40;

	float m_Spacing = 10;
	int m_AmountPerSide = 5u;

	bool m_UseCulling = false;

	bool* m_RenderingArray = nullptr;
	int m_AmountRendering = 0;
	int m_NumberOfFrustumChecks = 0;

	unsigned int m_InstanceArrayBuffer = 0;
	unsigned int m_InstanceArrayBufferShadow[NUM_OF_SHADOW_CASCADES] = { 0 };
	unsigned int m_NumberOfObjectsToRenderShadow[NUM_OF_SHADOW_CASCADES] = { 0 };
};

