#include "RenderMInstanced.h"




RenderMInstanced::RenderMInstanced(IRenderable* a_InstancedRenderable, Object* a_Objects, const int a_MaxDrawAmount) {
	m_Renderable = a_InstancedRenderable;
	m_DrawAmount = a_MaxDrawAmount;
}

RenderMInstanced::~RenderMInstanced() {
}

void RenderMInstanced::draw() {
	m_Renderable->drawInstance(m_DrawAmount);
}

void RenderMInstanced::drawInstance(unsigned int a_Amount) {
	m_Renderable->drawInstance(a_Amount);
}
