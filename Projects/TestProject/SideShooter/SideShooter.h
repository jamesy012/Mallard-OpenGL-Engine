#pragma once

#include "Application.h"

#include <vector>

class Model;
class Texture;
class Mesh;
class Text;
class Object;
class Shader;
class Framebuffer;

class Projectile;
class Player;
class SideShooterCamera;
class Camera;

class SideShooter :	public Application {
public:
	// Inherited via Application
	virtual void startUp() override;
	virtual void shutDown() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void drawUi() override;

private:
	void drawObjectInstanced(IRenderable* a_Renderable, glm::mat4* a_Array, int a_ArraySize);
	void drawObject(IRenderable* a_Renderable, bool a_Cull = true);

	void sceneRender(Camera* a_Camera, bool a_CloseOnly);

	Model* m_TreeModel;
	Mesh* m_Box;
	Model* m_QuadMesh;
	Model* m_PondMesh;
	Texture* m_GrassTexture;

	Shader* m_UniformShader;

	Text* m_TestText;

	Player* m_Player;

	Object* m_Ground;

	static const unsigned int NUM_OF_PONDS = 5;
	Object* m_Ponds[NUM_OF_PONDS];
	const float m_PondSize = 50.0f;

	SideShooterCamera* m_Camera;

	Framebuffer* m_ReflectionBuffer;
	Camera* m_ReflectionCamera;
	Shader* m_ReflectionShader;

	static const unsigned int NUM_OF_PROJECTILES = 50;
	Projectile* m_Projectiles[NUM_OF_PROJECTILES] = { nullptr };

	static const unsigned int NUM_OF_TREES = 200;
	unsigned int m_NumofTreesGenerated = NUM_OF_TREES;
	glm::mat4 m_UniformTrees[NUM_OF_TREES];
};

