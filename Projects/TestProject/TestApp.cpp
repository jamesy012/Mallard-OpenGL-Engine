#include "TestApp.h"

#include <stdio.h>


#include <gl\glew.h>

#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "TimeHandler.h"
#include "Window.h"
#include "Transform.h"

#include "Texture.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"

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

	m_Shader = new Shader();

	m_Shader->createSimpleShader(false);

	m_Mesh = new Mesh();
	m_Mesh->createBox();
	
	//delete t1;
	//delete t2;
	//delete t3;
	//delete t4;
}

void TestApp::shutDown() {
	m_Model->unload();
	delete m_Shader;
	delete m_Mesh;
}

void TestApp::update() {
	//printf("Window: (%i,%i)",m_AppWindow->getWindowWidth(),m_AppWindow->getWindowHeight());
	//
	//// Keep running
	//printf("Frame: %i dt: %f\n", TimeHandler::getCurrentFrameNumber(), TimeHandler::getDeltaTime());
}

void TestApp::draw() {
	m_Shader->use();

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), 16.0f/9.0f, 0.1f, 1000.0f);
	//glm::mat4 view = glm::lookAt(glm::vec3(1),glm::vec3(0),glm::vec3(0,1,0));
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0, 0, -10));
	view *= glm::rotate(TimeHandler::getCurrentTime(), glm::vec3(0, 1, 0));
	//view *= glm::rotate(TimeHandler::getCurrentTime() *0.25f, glm::vec3(1, 0, 0));

	glm::mat4 projectionView = projection*view;
	glm::mat4 model = glm::mat4(1.0f);

	unsigned int projectionViewMatrixLoc = glGetUniformLocation(m_Shader->getProgram(), "projectionViewMatrix");
	unsigned int modelMatrixLoc = glGetUniformLocation(m_Shader->getProgram(), "model");

	glUniformMatrix4fv(projectionViewMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionView));
	glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model));

	m_Model->draw();
	m_Mesh->draw();
}

void TestApp::drawUi() {
	
}
