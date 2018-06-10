#include "DepthOfField.h"

#include "Framebuffer.h"
#include "Shader.h"
#include "ShaderUniformData.h"

#include "Mesh.h"

#include "Texture.h"
#include <GL/glew.h>

#include "GLDebug.h"

const unsigned int m_ValueCount = 7;
const std::string m_ValueNames[m_ValueCount] = { "focusDistance", "falloff", "minStrength", "farDof", "nearDof", "farPlane", "nearPlane" };

DepthOfField::DepthOfField() {

}


DepthOfField::~DepthOfField() {
	if (m_DOF != nullptr) {
		delete m_DOF;
		delete m_DOFScaled;
		delete m_DOFScaledBlurred;

		delete m_DOFShader;
		delete m_BlurShader;
	}
}

void DepthOfField::create() {
	m_DOF = new Framebuffer();
	m_DOF->setSizePercent(1.0f, 1.0f);
	m_DOF->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGB, 16);
	m_DOF->genFramebuffer();
	GLDebug_NAMEOBJ(GL_FRAMEBUFFER, m_DOF->getFramebufferId(), "Depth of field result");
	GLDebug_NAMEOBJ(GL_TEXTURE, m_DOF->getTexture(0)->getTextureId(), "Depth of field result - RGB");


	m_DOFScaled = new Framebuffer();
	m_DOFScaled->setSizePercent(1 / m_ScaledSize, 1 / m_ScaledSize);
	m_DOFScaled->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGB, 16);
	m_DOFScaled->genFramebuffer();
	GLDebug_NAMEOBJ(GL_FRAMEBUFFER, m_DOFScaled->getFramebufferId(), "Depth of field Scaled 2(final)");
	GLDebug_NAMEOBJ(GL_TEXTURE, m_DOFScaled->getTexture(0)->getTextureId(), "Depth of field Scaled 2(final) - RGB");


	m_DOFScaledBlurred = new Framebuffer();
	m_DOFScaledBlurred->setSizePercent(1 / m_ScaledSize, 1 / m_ScaledSize);
	m_DOFScaledBlurred->addBuffer(FramebufferBufferTypes::TEXTURE, FramebufferBufferFormats::RGB, 16);
	m_DOFScaledBlurred->genFramebuffer();
	GLDebug_NAMEOBJ(GL_FRAMEBUFFER, m_DOFScaledBlurred->getFramebufferId(), "Depth of field Scaled 1");
	GLDebug_NAMEOBJ(GL_TEXTURE, m_DOFScaledBlurred->getTexture(0)->getTextureId(), "Depth of field Scaled 1 - RGB");



	m_BlurShader = new Shader();
	m_BlurShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
	m_BlurShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/GaussianBlur.frag");
	m_BlurShader->linkShader();
	//GLDebug_NAMEOBJ(GL_SHADER, m_BlurShader->getProgram(), "Blur Shader");


	m_DOFShader = new Shader();
	m_DOFShader->setFromPath(ShaderTypes::TYPE_VERTEX, "Shaders/PostProcessing/PPVertex.vert");
	m_DOFShader->setFromPath(ShaderTypes::TYPE_FRAGMENT, "Shaders/PostProcessing/DOF.frag");
	m_DOFShader->linkShader();
	//GLDebug_NAMEOBJ(GL_SHADER, m_DOFShader->getProgram(), "Depth of field Shader");


	m_FullScreenQuad = new Mesh();
	m_FullScreenQuad->createPlane(false);
	GLDebug_NAMEOBJ(GL_VERTEX_ARRAY, m_FullScreenQuad->m_Vao, "Depth of field full screen quad");


	for (int i = 0; i < m_ValueCount; i++) {
		ShaderUniformData* sud;
		sud = m_DOFShader->getUniform(ShaderUniformTypes::FLOAT, m_ValueNames[i].c_str());
		if (sud == nullptr) {
			printf("DOF m_ValueNames %i not in shader (%s)\n", i, m_ValueNames[i].c_str());
		} else {
			m_ValueMap[m_ValueNames[i]] = sud;
		}
	}
}

void DepthOfField::use(Framebuffer* a_ReadBuffers) {
	Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, false);

	const Shader* currentShader = Shader::getCurrentShader();
	const Framebuffer* currentFB = Framebuffer::getCurrentFramebuffer();

	//copy to downscaled
	{
		Framebuffer::framebufferBlit(a_ReadBuffers, m_DOFScaled);
	}
	//blur
	{
		Shader::use(m_BlurShader);

		ShaderUniformData* radiusUniform = m_BlurShader->getUniform(ShaderUniformTypes::FLOAT, "radius");
		ShaderUniformData* dirUniform = m_BlurShader->getUniform(ShaderUniformTypes::VEC2, "dir");

		glm::vec2 dir[2] = { glm::vec2(1,0),glm::vec2(0,1) };
		float radius[2] = { .2f, .2f };

		//run 1
		Framebuffer::use(m_DOFScaledBlurred);

		dirUniform->setData(&dir[0]);
		radiusUniform->setData(&radius[0]);

		Shader::checkUniformChanges();

		m_FullScreenQuad->setTexture(m_DOFScaled->getTexture());
		m_FullScreenQuad->draw();

		//run 2
		Framebuffer::use(m_DOFScaled);

		dirUniform->setData(&dir[1]);
		radiusUniform->setData(&radius[1]);

		Shader::checkUniformChanges();

		m_FullScreenQuad->setTexture(m_DOFScaledBlurred->getTexture());
		m_FullScreenQuad->draw();
	}
	//mix the result of the normal frame and blurred frame using the depth texture
	{
		Shader::use(m_DOFShader);
		Framebuffer::use(m_DOF);

		Shader::checkUniformChanges();

		Texture::bindTexture(a_ReadBuffers->getTexture(), 1);
		Texture::bindTexture(m_DOFScaled->getTexture(), 2);
		Texture::bindTexture(a_ReadBuffers->getTexture(1), 3);

		int loc = glGetUniformLocation(Shader::getCurrentShader()->getProgram(), "normalTex");
		glUniform1i(loc, 1);
		loc = glGetUniformLocation(Shader::getCurrentShader()->getProgram(), "blurredTex");
		glUniform1i(loc, 2);
		loc = glGetUniformLocation(Shader::getCurrentShader()->getProgram(), "depthTex");
		glUniform1i(loc, 3);

		m_FullScreenQuad->setTexture(nullptr);
		m_FullScreenQuad->draw();

		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Shader::use(currentShader);
	Framebuffer::use(currentFB);

	Framebuffer::glCall(Framebuffer::GL_CALLS::DEPTH_TEST, true);
}

void DepthOfField::setValue(const std::string a_UniformName, float a_NewValue) {
	auto pos = m_ValueMap.find(a_UniformName);
	if (pos != m_ValueMap.end()) {
		(*pos).second->setData(&a_NewValue);
	} else {
		printf("Depth of field: cant setValue because uniform %s does not exist.\n", a_UniformName.c_str());
	}
}

float DepthOfField::getValue(const std::string a_UniformName) {
	auto pos = m_ValueMap.find(a_UniformName);
	if (pos != m_ValueMap.end()) {
		return *(float*)((*pos).second->getDataVoid());
	} else {
		printf("Depth of field: cant getValue because uniform %s does not exist.\n", a_UniformName.c_str());
	}
}

unsigned int DepthOfField::getDOFTexture() {
	return m_DOF->getTexture()->getTextureId();
}

Framebuffer* DepthOfField::getDOFFramebuffer() {
	return m_DOF;
}
