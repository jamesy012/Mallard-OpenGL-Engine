#pragma once

#include <glm\glm.hpp>

class SideShooter;
class Player;
class Model;
class Enemy;

#include "SideShooterContants.h"

enum class EnemyTypes {
	STATIONARY = 0,
	SIDER,

	NUM_OF_ENEMY_TYPES
};

class EnemySpawner {
public:
	EnemySpawner(SideShooter* a_App, Player* a_Player);
	~EnemySpawner();

	void update();

	//we store a loaded reference to all enemies meshs, so we dont have to reload the mesh from file
	Model* m_EnemyStationaryMesh;
	Model* m_EnemySideMesh;
	

	//reference to the first model in memory
	Model** m_FirstModel = &m_EnemyStationaryMesh;
private:
	Enemy* spawnOnSideofPlayer(EnemyTypes a_EnemyType, bool a_RightSide, glm::vec3 a_Offset, float a_XPlayerOffset = SSConstants::SCREEN_SIDE_SPAWN_DIST);

	SideShooter* m_App;
	Player* m_player;

};

