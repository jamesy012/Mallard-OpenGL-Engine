#include "Framebuffer.h"

#include <GL\glew.h>
#include <stdio.h>

#include "Texture.h"
#include "Window.h"

Framebuffer::Framebuffer() {
	m_FboTexture = nullptr;
	m_Fbo = m_RboDepth = 0;
	m_Width = m_Height = 1;//1x1 starting size
}


Framebuffer::~Framebuffer() {
	if (m_FboTexture != nullptr) {
		delete m_FboTexture;
	}
	//check fbo
	if (m_Fbo != 0) {
		glDeleteFramebuffers(1, &m_Fbo);
		m_Fbo = 0;
	}
	//check depth
	if (m_RboDepth != 0) {
		glDeleteRenderbuffers(1, &m_RboDepth);
		m_RboDepth = 0;
	}
}

void Framebuffer::setSize(const unsigned int a_Width, const unsigned int a_Height) {
	m_Width = a_Width;
	m_Height = a_Height;
}

void Framebuffer::use(Framebuffer * a_Framebuffer) {
	if (a_Framebuffer == nullptr) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Window* window = Window::getMainWindow();
		glViewport(0, 0, window->getFramebufferWidth(), window->getFramebufferHeight());
		glClearColor(0.75f, 0.0f, 0.75f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, a_Framebuffer->m_Fbo);
	glViewport(0, 0, a_Framebuffer->m_Width, a_Framebuffer->m_Height);
	//glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::genFramebuffer() {
	unsigned int textureID = 0;
	//create texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_Width, m_Height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	//set up Texture object
	m_FboTexture = new Texture(textureID, m_Width, m_Height);

	//depth buffer
	glGenRenderbuffers(1, &m_RboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height);

	//frame buffer
	glGenFramebuffers(1, &m_Fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

	//bind the above
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RboDepth);

	//color attachments
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	//simple error checker
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer Error\n");
	}

	//done, lets unbind the buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture * Framebuffer::getTexture() const {
	return m_FboTexture;
}
