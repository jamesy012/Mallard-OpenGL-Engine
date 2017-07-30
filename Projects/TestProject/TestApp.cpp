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

#include "Font.h"

void TestApp::startUp() {
	//Texture* t1 = new Texture();
	//Texture* t2 = new Texture();
	//Texture* t3 = new Texture();
	//Texture* t4 = new Texture();
	m_Texture = new Texture();

	//m_Texture->load("Textures/imageTest.jpg");
	m_Texture->load("Textures/imageTest.png");
	//m_Texture->load1x1Texture();
	//t1->load("Textures/imageTest.png");
	//t2->load("test2");
	//t3->load("Textures/imageTest.png");
	//t4->load("Textures/imageTest.png");
	//
	//t1->unload();
	//t2->unload();
	//t3->unload();
	//t4->unload();

	m_Model = new Model();

	//m_Model->load("Models/Nanosuit/nanosuit.obj");
	//m_Model->load("Models/ModelTest/ModelTest.fbx");
	m_Model->load("Models/ModelTest/ModelTest.obj");


	m_Shader = new Shader();

	m_Shader->createSimpleShader(true);

	m_Mesh = new Mesh();
	m_Mesh->createBox();
	m_Mesh->setTexture(m_Texture);

	//delete t1;
	//delete t2;
	//delete t3;
	//delete t4;

	m_Font = new Font();
	m_Font->loadFont("c:/windows/fonts/comic.ttf", 48);
	m_Font->genText("~ Test Text, 1234!@#$ ~\nNew Line");

	//todo, move to Font
	const char* textVertex = R"(
            #version 330 core
			layout(location = 0) in vec4 position;
			layout(location = 2) in vec2 texCoord0;

			uniform mat4 projectionViewMatrix;
			uniform mat4 model = mat4(1);

			        out vec2 uv0;
            void main()
	        {
	            gl_Position = projectionViewMatrix * model * position;
	            uv0 = texCoord0;
	        }
        )";
	const char* textFragment = R"(
	        #version 330 core
            uniform sampler2D TexDiffuse1;
			uniform vec4 color = vec4(1,1,1,1);
            in vec2 uv0;
	        out vec4 fragColor;
            void main()
	        {
                vec4 c = texture(TexDiffuse1, uv0);
    	        fragColor = c.rrrr * color;
	        }
	    )";

	m_TextShader = new Shader();
	m_TextShader->setFromText(ShaderTypes::TYPE_VERTEX, textVertex);
	m_TextShader->setFromText(ShaderTypes::TYPE_FRAGMENT, textFragment);
	m_TextShader->linkShader();
}

void TestApp::shutDown() {
	m_Model->unload();
	m_Texture->unload();
	delete m_Shader;
	delete m_Mesh;
	delete m_Font;
	delete m_TextShader;
}

void TestApp::update() {
	//printf("Window: (%i,%i)",m_AppWindow->getWindowWidth(),m_AppWindow->getWindowHeight());
	//
	//// Keep running
	//printf("Frame: %i dt: %f\n", TimeHandler::getCurrentFrameNumber(), TimeHandler::getDeltaTime());
}

void TestApp::draw() {
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 projectionView;
	Transform model;

	ShaderUniformData* uniformPVM;
	ShaderUniformData* uniformModel;

	///NORMAL RENDER
	m_Shader->use();

	//set up data
	projection = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
	view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0, -2, -20));
	view *= glm::rotate(TimeHandler::getCurrentTime(), glm::vec3(0, 1, 0));
	projectionView = projection*view;
	
	model.rotate(glm::vec3(TimeHandler::getCurrentTime()*0.32f, 0, 0) * 100.0f);

	//get uniforms
	uniformPVM = m_Shader->m_CommonUniforms.m_ProjectionViewMatrix;
	uniformModel = m_Shader->m_CommonUniforms.m_ModelMatrix;

	//set data
	uniformPVM->setData(&projectionView);
	uniformModel->setData(&model);

	//apply uniforms
	Shader::applyUniform(uniformPVM);
	Shader::applyUniform(uniformModel);

	//draw models
	m_Model->draw();
	m_Mesh->draw();

	///TEXT RENDER

	ShaderUniformData* uniformColor;

	m_TextShader->use();
	//get uniforms
	uniformColor = m_TextShader->m_CommonUniforms.m_Color;
	uniformPVM = m_TextShader->m_CommonUniforms.m_ProjectionViewMatrix;

	view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(-32, -16, -400));
	view = glm::translate(view, glm::vec3((sin(TimeHandler::getCurrentTime() / 1.83f)*0.5f + 0.5f) * -350, 200, 0));
	projectionView = projection*view;


	uniformPVM->setData(&projectionView);
	//change just the alpha of the uniform
	uniformColor->modifyData(3, { (sin(TimeHandler::getCurrentTime())*0.5f + 0.5f) });

	Shader::applyUniform(uniformPVM);
	Shader::applyUniform(uniformColor);

	//glDisable(GL_CULL_FACE);
	//glDepthMask(GL_TRUE);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Font->draw();
	//m_Model->draw();

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void TestApp::drawUi() {
	//m_Font->draw();
}
