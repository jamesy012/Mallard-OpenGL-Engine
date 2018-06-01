#include "DepthOfField.h"

#include "Framebuffer.h"
#include "Shader.h"
#include "Renderer/RenderMList.h"

#include "Mesh.h"

#include "Texture.h"
#include <GL/glew.h>

DepthOfField::DepthOfField() {

}


DepthOfField::~DepthOfField() {
	if (m_DOF != nullptr) {
		delete m_DOF;
		delete m_DOFScaled;
		delete m_DOFGenShader;
		delete m_DOFDrawShader;
	}
}

void DepthOfField::create() {
	m_DOF = new Framebuffer();
	m_DOF->setSize(m_Width, m_Height);
	m_DOF->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGB, 16);
	m_DOF->genFramebuffer();
	m_DOFScaled = new Framebuffer();
	m_DOFScaled->setSize(m_Width / m_ScaledSize, m_Height / m_ScaledSize);
	m_DOFScaled->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGB, 16);
	m_DOFScaled->genFramebuffer();
	m_DOFScaledBlurred = new Framebuffer();
	m_DOFScaledBlurred->setSize(m_Width / m_ScaledSize, m_Height / m_ScaledSize);
	m_DOFScaledBlurred->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGB, 16);
	m_DOFScaledBlurred->genFramebuffer();

	m_DOFGenShader = new Shader();
	m_DOFGenShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
	m_DOFGenShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/DOFGen.frag");
	m_DOFGenShader->linkShader();
	m_DOFDrawShader = new Shader();
	m_DOFDrawShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
	m_DOFDrawShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/DOFDraw2.frag");
	m_DOFDrawShader->linkShader();

	m_BlurShader = new Shader();
	m_BlurShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
	m_BlurShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/GaussianBlur.frag");
	m_BlurShader->linkShader();

	m_FullScreenQuad = new Mesh();
	m_FullScreenQuad->createPlane(false);
}

void DepthOfField::use(Camera* a_Camera, RenderMList * a_List) {
	Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, false);

	const Shader* currentShader = Shader::getCurrentShader();
	const Framebuffer* currentFB = Framebuffer::getCurrentFramebuffer();

	//generate dof data
	{
		Shader::use(m_DOFGenShader);
		Framebuffer::use(m_DOF);

		if (m_ReadBuffer != nullptr) {
			m_FullScreenQuad->setTexture(m_ReadBuffer->getTexture(1));
			m_FullScreenQuad->draw();
		}
	}
	//scale it down
	{
		Framebuffer::framebufferBlit(m_DOF, m_DOFScaled);
	}
	//blur it
	{
		Shader::use(m_BlurShader);
		Framebuffer::use(m_DOFScaledBlurred);

		ShaderUniformData* radiusUniform = m_BlurShader->getUniform(ShaderUniformTypes::FLOAT, "radius");
		ShaderUniformData* dirUniform = m_BlurShader->getUniform(ShaderUniformTypes::VEC2, "dir");

		glm::vec2 dir[2] = { glm::vec2(1,0),glm::vec2(0,1) };
		float radius[2] = { .3f,.45f };
		dirUniform->setData(&dir[0]);
		radiusUniform->setData(&radius[0]);

		Shader::checkUniformChanges();

		m_FullScreenQuad->setTexture(m_DOFScaled->getTexture());
		m_FullScreenQuad->draw();

		Framebuffer::use(m_DOFScaled);

		dirUniform->setData(&dir[1]);
		radiusUniform->setData(&radius[1]);

		Shader::checkUniformChanges();

		Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, false);
		m_FullScreenQuad->setTexture(m_DOFScaledBlurred->getTexture());
		m_FullScreenQuad->draw();
	}
	//use blured data and orginal image to create final image with dof
	{
		Shader::use(m_DOFDrawShader);
		Framebuffer::use(m_DOF);

		Texture::bindTexture(m_ReadBuffer->getTexture(), 1);
		Texture::bindTexture(m_DOFScaled->getTexture(), 2);

		int loc = glGetUniformLocation(Shader::getCurrentShader()->getProgram(), "TexDiffuse1");
		glUniform1i(loc, 1);
		loc = glGetUniformLocation(Shader::getCurrentShader()->getProgram(), "depthTexture");
		glUniform1i(loc, 2);

		m_FullScreenQuad->setTexture(nullptr);
		m_FullScreenQuad->draw();

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Shader::use(currentShader);
	Framebuffer::use(currentFB);

	Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, true);
}
