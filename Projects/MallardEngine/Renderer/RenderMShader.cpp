#include "RenderMShader.h"

#include "..\\Shader.h"

RenderMShader::RenderMShader(std::string a_Name, Shader* a_Shader, Camera* a_ProjectionViewCamera) {
	m_Name = a_Name;
	m_Shader = a_Shader;
	m_Camera = a_ProjectionViewCamera;
}

void RenderMShader::use() {
	Shader::use(m_Shader);

	ShaderUniformData* pvmUniform = m_Shader->m_CommonUniforms.m_ProjectionViewMatrix;
	pvmUniform->setData(m_Camera);

	Shader::checkUniformChanges();
}

void RenderMShader::draw() {
	use();
}

void RenderMShader::drawInstance(unsigned int a_Amount) {
	_ASSERT_EXPR(false, L"RenderMShader has no Instanced Draw");
}
