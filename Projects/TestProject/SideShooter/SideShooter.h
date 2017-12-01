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
class EnemySpawner;

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

	unsigned int m_NumOfEnemiesAlive = 0;

	//list of application option flags
	struct {
		bool m_EnableDof = true;
		bool m_DisplayUI = true;
	} m_AppOptions;
private:
	void drawObjectInstanced(IRenderable* a_Renderable, glm::mat4* a_Array, int a_ArraySize);
	bool drawObject(IRenderable* a_Renderable, bool a_Cull = true, float a_CullOffset  = 0);

	void runCollisionCheck();

	void sceneRender(bool a_CloseOnly, bool a_IncludeGround);

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
	bool m_DoRenderPonds = true;

	//Depth of field
	Framebuffer* m_DepthOfFieldTest;
	Shader* m_DOFGenShader;
	Shader* m_DOFDrawShader;

	//Shadows
	Framebuffer* m_ShadowDirectionalLightFb;
	Shader* m_ShadowGenInstancedShader;
	Shader* m_ShadowDrawInstancedShader;
	Camera* m_ShadowDirectionalCamera;
	glm::vec3 m_LightDir = glm::normalize(glm::vec3(-2.0f, -2.5f, -3.0f));
	const float m_ShadowMapSize = 100.0f;

	//common object arrays
	static const unsigned int MAX_NUM_OF_PROJECTILES = 50;
	Projectile* m_Projectiles[MAX_NUM_OF_PROJECTILES] = { nullptr };

	static const unsigned int NUM_OF_TREES = 384u;
	unsigned int m_NumofTreesGenerated = NUM_OF_TREES;
	glm::mat4 m_UniformTrees[NUM_OF_TREES];

	//enemys
	EnemySpawner* m_EnemySpawner;

	static const unsigned int MAX_NUM_OF_ENEMIES = 50;
	Enemy* m_Enemies[MAX_NUM_OF_ENEMIES] = { nullptr };


	//dof intro changer
	bool m_IsDoingDofIntro = true;
	const float m_TargetDof = 95.0f;
	float m_StartingDof = 200.0f;
	float m_DofChangeStartTime = -1;
	const float m_DofChangeDuration = 5.0f;
};

