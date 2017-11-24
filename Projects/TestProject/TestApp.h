#pragma once

#include "Application.h"

class Model;
class Text;

class TestApp :
	public Application {
public:
	// Inherited via Application
	virtual void startUp() override;
	virtual void shutDown() override;
	virtual void update() override;
	virtual void draw() override;
	virtual void drawUi() override;

private:

	Model* m_Model;

	Text* m_TestText;
};

