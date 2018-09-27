#pragma once

#include "DLLBuild.h"
#include "IRenderable.h"

#include <glm\glm.hpp>

#include <string>

class Shader;
class Camera;

//some duplications of Texture and Mesh, should be fine tho

class DLL_BUILD Skybox : public IRenderable {
public:
	Skybox();
	~Skybox();

	void genSkybox(const std::string a_Front, const std::string a_Back,
				   const std::string a_Top, const std::string a_Bottom,
				   const std::string a_Left, const std::string a_Right);
	//loads 6 images from a_Path, all jpg
	//negx, negy, negz, posx, posy, posz
	void genSkybox(const std::string a_Path);

	void assignCamera(Camera* a_AssignedCamera);

	bool hasCameraAssigned() {
		return m_Camera != nullptr;
	}

	bool hasBeenGenerated() {
		return m_VAO != 0;
	}

	// Inherited via IRenderable
	virtual void draw() override;
	void draw(Camera& a_Camera);
	virtual void drawInstance(unsigned int a_Amount) override;

protected:
	void genMesh();
	void genShader();
	void addSkyboxSide(unsigned int a_SideTarget, const std::string a_FilePath);

	Shader* m_SkyboxShader = nullptr;
	Camera* m_Camera = nullptr;
	glm::mat4 m_CameraPV;
	unsigned int m_LastCameraUpdate = -1;

	unsigned int m_VAO, m_VBO, m_TextureID;
};

