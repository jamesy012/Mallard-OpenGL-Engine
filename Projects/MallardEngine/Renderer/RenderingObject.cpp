#include "RenderingObject.h"


#include <GL\glew.h>
#include "Logging.h"

#include "Object.h"
#include "Shader.h"
#include "ShaderUniformData.h"

RenderingObject::RenderingObject() : RenderingBase("Rendering: Basic Draw") {
}

RenderingObject::~RenderingObject() {
}


void RenderingObject::draw() {
	if (m_Object == nullptr) {
		return;
	}
	Renderable* ro = m_Object->m_Renderable;
	if (ro == nullptr) {
		return;
	}
	ShaderUniformData* uniformModel = Shader::getCurrentShader()->m_CommonUniforms.m_ModelMatrix;
	uniformModel->setData(&m_Object->m_Transform);
	Shader::applyUniform(uniformModel);

	if (ro->m_Vao == 0) {
		return;
	}

	//todo: textures
	//unsigned int loc = 0;
	//if (m_Texture != nullptr) {
	//	//todo move this out of mesh
	//	int slot = 0;
	//	Texture::bindAndApplyTexture(m_Texture, slot, Shader::getCurrentShader()->m_CommonUniforms.m_Textures[0]);
	//}

	glBindVertexArray(ro->m_Vao);
	glDrawElements(GL_TRIANGLES, ro->m_IndicesCount, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);

	//unbind texture
	//glActiveTexture(GL_TEXTURE0 + 0);
	//glBindTexture(GL_TEXTURE_2D, 0);

	Logging::objectRendered(ro->m_IndicesCount);
}

