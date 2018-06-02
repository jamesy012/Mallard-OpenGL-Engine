#include "Object.h"

#include "Shader.h"

Object::Object(IRenderable* a_Renderable) {
	m_Renderable = a_Renderable;
}

Object::Object(IRenderable* a_Renderable, const char * a_TransformName) : Object(a_Renderable) {
	m_Transform.m_Name = a_TransformName;
}

Object::~Object() {}
