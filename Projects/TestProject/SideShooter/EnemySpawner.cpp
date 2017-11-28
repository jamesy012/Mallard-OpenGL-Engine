#include "EnemySpawner.h"

#include "Model.h"

#include "Player.h"
#include "SideShooter.h"

EnemySpawner::EnemySpawner(SideShooter * a_App, Player* a_Player) {
	m_App = a_App;
	m_player = a_Player;

	m_EnemyStationaryMesh = new Model();
	m_EnemyStationaryMesh->load("Models\\SideShooter\\Enemy1.obj");
}

EnemySpawner::~EnemySpawner() {
	m_EnemyStationaryMesh->unload();
}

void EnemySpawner::update() {
	if (m_App->m_NumOfEnemiesAlive == 0) {
		for (int i = 0; i < 4; i++) {
			glm::vec3 offset = glm::vec3(0, -SSConstants::GROUND_Y + 5 + (i * 5), 0);
			spawnOnSideofPlayer(true, offset);
			spawnOnSideofPlayer(false, offset);
		}
	}
}

void EnemySpawner::spawnOnSideofPlayer(bool a_RightSide, glm::vec3 a_Offset, float a_XPlayerOffset) {
	m_App->spawnEnemy(glm::vec3(m_player->m_Transform.getGlobalPosition().x + (a_RightSide?1:-1) * a_XPlayerOffset, 0, 0) + a_Offset);
}
