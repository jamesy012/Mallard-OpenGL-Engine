#pragma once

#include "Application.h"

#include <vector>

class Model;
class Texture;
class Mesh;
class MeshBatch;
class Text;
class Object;
class Shader;
class Framebuffer;

class Projectile;
class Player;
class SideShooterCamera;
class Camera;

class Enemy;

#include "EnemySpawner.h"

#define USE_ONE_TREE_MODEL true

class SideShooter :	public Application {
public:

	SideShooter();

	// Inherited via Application
	virtual void startUp() override;
	virtual void shutDown() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void drawUi() override;

	//spawn a projecile at a_Position
	Projectile* spwanProjectile(Transform* a_Position, bool a_Right, IRenderable* a_Model);
	Enemy* spawnEnemy(EnemyTypes a_EnemyType, glm::vec3 a_Position);

	unsigned int m_NumOfEnemiesAlive[(unsigned int)EnemyTypes::NUM_OF_ENEMY_TYPES] = { 0 };

	//list of application option flags
	struct {
		bool m_EnableDof = true;
		bool m_DisplayUI = true;
		bool m_InstanceTreeRender = false;
	} m_AppOptions;

private:
	static void drawObjectInstanced(IRenderable* a_Renderable, glm::mat4* a_Array, int a_ArraySize);
	bool drawObject(IRenderable* a_Renderable, bool a_Cull = true, float a_CullOffset  = 0);

	void runCollisionCheck();

	void sceneRender(bool a_CloseOnly, bool a_IncludeGround);

	static const unsigned int NUM_OF_TREES = 512u;
	static const unsigned int NUM_OF_TREE_MODELS = 4u;
	static const unsigned int NUM_OF_TREES_PER_MODEL = NUM_OF_TREES / NUM_OF_TREE_MODELS;

	//Model* m_TreeModel;
	Model* m_TreeModels[NUM_OF_TREE_MODELS];
	Mesh* m_Box;
	Model* m_QuadMesh;
	Model* m_PondMesh;
	Texture* m_GrassTexture;
	MeshBatch* m_StaticTerrainMesh;
	MeshBatch* m_StaticTerrainMeshCloseOnly;

	Shader* m_UniformShader;

	Text* m_TestText;

	Player* m_Player;

	Object* m_Ground;
	Object* m_StaticTerrainObject;

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
	Framebuffer* m_DepthOfFieldTestScaled;
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


	unsigned int m_NumofTreesGenerated = NUM_OF_TREES;
	glm::mat4 m_UniformTrees[NUM_OF_TREES];
	glm::mat4 m_UniformTreesSorted[NUM_OF_TREES];

	//enemys
	EnemySpawner* m_EnemySpawner;

	static const unsigned int MAX_NUM_OF_ENEMIES_PER_TYPE = 10;
	Enemy* m_Enemies[(unsigned int)EnemyTypes::NUM_OF_ENEMY_TYPES][MAX_NUM_OF_ENEMIES_PER_TYPE] = { nullptr };


	//dof intro changer
	bool m_IsDoingDofIntro = true;
	const float m_TargetDof = 95.0f;
	float m_StartingDof = 200.0f;
	float m_DofChangeStartTime = -1;
	const float m_DofChangeDuration = 5.0f;

};

