#pragma once

#include "Application.h"

class Model;
class Mesh;
class Text;
class Object;
class Camera;

class Projectile;
class Player;

class SideShooter :	public Application {
public:
	// Inherited via Application
	virtual void startUp() override;
	virtual void shutDown() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void drawUi() override;

private:

	Model* m_TreeModel;
	Mesh* m_Box;
	Mesh* m_QuadMesh;

	Text* m_TestText;


	Player* m_Player;
	Object* m_Standard;

	Object* m_Ground;

	Camera* m_Camera;

	static const unsigned int NUM_OF_PROJECTILES = 20;
	Projectile* m_Projectiles[NUM_OF_PROJECTILES] = { nullptr };
};

