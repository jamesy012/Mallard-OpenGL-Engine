#include "TestApp.h"

#include <stdio.h>
#include "TimeHandler.h"
#include "Window.h"
#include "Transform.h"

#include "Texture.h"
#include "Model.h"

void TestApp::startUp() {
	Texture* t1 = new Texture();
	Texture* t2 = new Texture();
	Texture* t3 = new Texture();
	Texture* t4 = new Texture();

	t1->load("test1");
	t2->load("test2");
	t3->load("test1");
	t4->load("test1");

	t1->unload();
	t2->unload();
	t3->unload();
	t4->unload();

	m_Model = new Model();

	m_Model->load("Models/Nanosuit/nanosuit.obj");

	//delete t1;
	//delete t2;
	//delete t3;
	//delete t4;
}

void TestApp::shutDown() {
	m_Model->unload();
}

void TestApp::update() {
	//printf("Window: (%i,%i)",m_AppWindow->getWindowWidth(),m_AppWindow->getWindowHeight());
	//
	//// Keep running
	//printf("Frame: %i dt: %f\n", TimeHandler::getCurrentFrameNumber(), TimeHandler::getDeltaTime());
}

void TestApp::draw() {
	m_Model->draw();
}

void TestApp::drawUi() {
	
}
