#include "RenderMList.h"

#include "Shader.h"

#include "Object.h"

void RenderMList::draw() {
	ShaderUniformData* uniformModel = Shader::getCurrentShader()->m_CommonUniforms.m_ModelMatrix;

	for (size_t i = 0; i < m_RenderList.size(); i++) {

		uniformModel->setData(&m_RenderList[i]->m_Transform);
		Shader::applyUniform(uniformModel);

		m_RenderList[i]->m_Renderable->draw();
	}
}

void RenderMList::drawInstance(unsigned int a_Amount) {
	_ASSERT_EXPR(false, L"RenderMList has no Instanced Draw");
}

void RenderMList::addObject(Object * a_Renderable) {
	m_RenderList.push_back(a_Renderable);
}
