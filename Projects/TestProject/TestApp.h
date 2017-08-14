#pragma once

#include "Application.h"

class Model;
class Mesh;
class Shader;
class Texture;
class Font;
class Text;
class Framebuffer;

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

	void runFramebufferTest();

	Model* m_Model;
	Mesh* m_Mesh;
	Shader* m_Shader;
	Shader* m_TextShader;
	Shader* m_PostprocessingBlur;
	Texture* m_Texture;
	Font* m_Font;
	Text* m_TestText;

	Camera* m_FbCamera;
	Mesh* m_FbPlane;
	Framebuffer* m_FbTest;
};

