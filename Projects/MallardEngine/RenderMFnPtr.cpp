#include "Renderer\RenderMFnPtr.h"



RenderMFnPtr::RenderMFnPtr() {
	m_Function = nullptr;
}

RenderMFnPtr::RenderMFnPtr(RendedererFnPtr a_Function) {
	m_Function = a_Function;
}


RenderMFnPtr::~RenderMFnPtr() {
}

void RenderMFnPtr::draw() {
	m_Function();
}

void RenderMFnPtr::drawInstance(unsigned int a_Amount) {
	_ASSERT(false);
}
