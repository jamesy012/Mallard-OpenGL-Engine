#include "TestApp.h"

#include <stdio.h>
#include "MallardEngine\TimeHandler.h"

void TestApp::startUp() {
}

void TestApp::shutDown() {
	
}

void TestApp::update() {
	// Keep running
	printf("Frame: %i dt: %f\n", TimeHandler::getCurrentFrameNumber(), TimeHandler::getDeltaTime());
}

void TestApp::draw() {
	
}

void TestApp::drawUi() {
	
}
