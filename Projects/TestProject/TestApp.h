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

	Model* m_Model;

	Text* m_TestText;

	Texture* m_1x1WhiteTexture;
};

