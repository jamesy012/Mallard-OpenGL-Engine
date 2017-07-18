#pragma once

#include "Application.h"

class Model;
class Mesh;
class Shader;
class Texture;
class Font;

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
	Mesh* m_Mesh;
	Shader* m_Shader;
	Shader* m_TextShader;
	Texture* m_Texture;
	Font* m_Font;
};

