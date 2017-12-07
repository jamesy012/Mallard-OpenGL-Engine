#include "EnemySpawner.h"

#include "Model.h"

#include "Player.h"
#include "SideShooter.h"

#include "Enemys\EnemySider.h"

EnemySpawner::EnemySpawner(SideShooter * a_App, Player* a_Player) {
	m_App = a_App;
	m_player = a_Player;

	m_EnemyStationaryMesh = new Model();
	m_EnemyStationaryMesh->load("Models\\SideShooter\\Enemy1.obj");

	m_EnemySideMesh = new Model();
	m_EnemySideMesh->load("Models\\SideShooter\\Enemy2.obj");

}

EnemySpawner::~EnemySpawner() {
	m_EnemyStationaryMesh->unload();
	m_EnemySideMesh->unload();
}

void EnemySpawner::update() {
	if (m_App->m_NumOfEnemiesAlive[(int)EnemyTypes::STATIONARY] == 0) {
		for (int i = 0; i < 4; i++) {
			glm::vec3 offset = glm::vec3(0, -SSConstants::GROUND_Y + 5 + (i * 5), 0);
			spawnOnSideofPlayer(EnemyTypes::STATIONARY, true, offset);
			spawnOnSideofPlayer(EnemyTypes::STATIONARY, false, offset);
		}
	}

	if (m_App->m_NumOfEnemiesAlive[(int)EnemyTypes::SIDER] == 0) {
		glm::vec3 offset = glm::vec3(0, -SSConstants::GROUND_Y + 5 + 5, 0);
		EnemySider* eL = (EnemySider*) spawnOnSideofPlayer(EnemyTypes::SIDER, false, offset, 40);
		EnemySider* eR = (EnemySider*) spawnOnSideofPlayer(EnemyTypes::SIDER, true, offset, 40);
		eL->m_RightSide = false;
	}

}

Enemy* EnemySpawner::spawnOnSideofPlayer(EnemyTypes a_EnemyType, bool a_RightSide, glm::vec3 a_Offset, float a_XPlayerOffset) {
	return m_App->spawnEnemy(a_EnemyType, glm::vec3(m_player->m_Transform.getGlobalPosition().x + (a_RightSide ? 1 : -1) * a_XPlayerOffset, 0, 0) + a_Offset);
}
