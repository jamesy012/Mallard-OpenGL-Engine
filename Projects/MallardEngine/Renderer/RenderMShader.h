#pragma once
#include "DLLBuild.h"

#include <string>

#include "..\\IRenderable.h"

class Camera;
class Shader;

class DLL_BUILD RenderMShader : public IRenderable {
	RenderMShader(std::string a_Name, Shader* a_Shader, Camera* a_ProjectionViewCamera);

	void use();

	// Inherited via IRenderable
	virtual void draw() override;
	virtual void drawInstance(unsigned int a_Amount) override;

	std::string m_Name;
	Shader* m_Shader;
	Camera* m_Camera;
};

