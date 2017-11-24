#include "Object.h"

#include "Shader.h"

Object::Object() {}

Object::Object(const char * a_TransformName) {
	m_Transform.m_Name = a_TransformName;
}

Object::~Object() {}

void Object::draw() {
	if (m_Renderable != nullptr) {
		ShaderUniformData* modelUniform = Shader::getCurrentShader()->m_CommonUniforms.m_ModelMatrix;
		if (modelUniform != nullptr) {
			glm::mat4 modelRot = m_Transform.getGlobalMatrix() * m_Renderable->getTransformMatrix();
			modelUniform->setData(&modelRot);
			Shader::applyUniform(modelUniform);
		}
		m_Renderable->draw();
	}
}

void Object::drawInstance(unsigned int a_Amount) {
	m_Renderable->drawInstance(a_Amount);
}
