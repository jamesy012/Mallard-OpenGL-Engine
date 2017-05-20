#include "TestApp.h"

#include <stdio.h>
#include "TimeHandler.h"
#include "Window.h"
#include "Transform.h"

void TestApp::startUp() {
}

void TestApp::shutDown() {
}

void TestApp::update() {
	printf("Window: (%i,%i)",m_AppWindow->getWindowWidth(),m_AppWindow->getWindowHeight());

	// Keep running
	printf("Frame: %i dt: %f\n", TimeHandler::getCurrentFrameNumber(), TimeHandler::getDeltaTime());
}

void TestApp::draw() {
	
}

void TestApp::drawUi() {
	
}
