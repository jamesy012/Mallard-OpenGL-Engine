#pragma once

#include <glm\glm.hpp>

class SideShooter;
class Player;
class Model;

#include "SideShooterContants.h"

class EnemySpawner {
public:
	EnemySpawner(SideShooter* a_App, Player* a_Player);
	~EnemySpawner();

	void update();

	//we store a loaded reference to all enemies meshs, so we dont have to reload the mesh from file
	Model* m_EnemyStationaryMesh;
private:
	void spawnOnSideofPlayer(bool a_RightSide, glm::vec3 a_Offset, float a_XPlayerOffset = SSConstants::SCREEN_SIDE_SPAWN_DIST);

	SideShooter* m_App;
	Player* m_player;

};

