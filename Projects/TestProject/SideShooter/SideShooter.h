#pragma once

#include "Application.h"

class Model;
class Texture;
class Mesh;
class Text;
class Object;

class Projectile;
class Player;
class SideShooterCamera;

class SideShooter :	public Application {
public:
	// Inherited via Application
	virtual void startUp() override;
	virtual void shutDown() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void drawUi() override;

private:
	void drawObject(IRenderable* a_Renderable, bool a_Cull = true);

	Model* m_TreeModel;
	Mesh* m_Box;
	Model* m_QuadMesh;
	Texture* m_GrassTexture;

	Text* m_TestText;

	Player* m_Player;

	Object* m_Ground;

	SideShooterCamera* m_Camera;

	static const unsigned int NUM_OF_PROJECTILES = 50;
	Projectile* m_Projectiles[NUM_OF_PROJECTILES] = { nullptr };

	//could just instance these, since they dont move
	//instancing hasent been set up to be done easily 
	static const unsigned int NUM_OF_TREES = 100;
	Object* m_Trees[NUM_OF_TREES] = { nullptr };
};

