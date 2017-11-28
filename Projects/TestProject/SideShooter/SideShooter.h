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

class Enemy;

class SideShooter :	public Application {
public:
	// Inherited via Application
	virtual void startUp() override;
	virtual void shutDown() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void drawUi() override;

	//spawn a projecile at a_Position
	Projectile* spwanProjectile(Transform* a_Position, bool a_Right, IRenderable* a_Model);
	Enemy* spawnEnemy(glm::vec3 a_Position);


	//list of application option flags
	struct {
		bool m_EnableDof = true;
		bool m_DisplayUI = true;
	} m_AppOptions;
private:
	void drawObjectInstanced(IRenderable* a_Renderable, glm::mat4* a_Array, int a_ArraySize);
	void drawObject(IRenderable* a_Renderable, bool a_Cull = true, float a_CullOffset  = 0);

	void runCollisionCheck();

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
	Framebuffer* m_ReflectionScaledBuffer;
	Camera* m_ReflectionCamera;
	Shader* m_ReflectionShader;
	Shader* m_BlurShader;

	Framebuffer* m_DepthOfFieldTest;
	Shader* m_DOFGenShader;
	Shader* m_DOFDrawShader;

	static const unsigned int MAX_NUM_OF_PROJECTILES = 50;
	Projectile* m_Projectiles[MAX_NUM_OF_PROJECTILES] = { nullptr };


	static const unsigned int NUM_OF_TREES = 250;
	unsigned int m_NumofTreesGenerated = NUM_OF_TREES;
	glm::mat4 m_UniformTrees[NUM_OF_TREES];

	//enemys
	//we store a loaded reference to all enemies meshs, so we dont have to reload the mesh from file
	Model* m_EnemyStationaryMesh;

	static const unsigned int MAX_NUM_OF_ENEMIES = 50;
	Enemy* m_Enemies[MAX_NUM_OF_ENEMIES] = { nullptr };

};

