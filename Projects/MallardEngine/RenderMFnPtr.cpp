#include "Renderer\RenderMFnPtr.h"



RenderMFnPtr::RenderMFnPtr() {
	m_Function = nullptr;
}

RenderMFnPtr::RenderMFnPtr(RendedererFnPtr a_Function, IRenderable* a_RenderObject) {
	m_Function = a_Function;
	m_RenderObject = a_RenderObject;
}


RenderMFnPtr::~RenderMFnPtr() {
}

void RenderMFnPtr::draw() {
	m_Function(1, m_RenderObject);
}

void RenderMFnPtr::drawInstance(unsigned int a_Amount) {
	m_Function(a_Amount, m_RenderObject);
}
