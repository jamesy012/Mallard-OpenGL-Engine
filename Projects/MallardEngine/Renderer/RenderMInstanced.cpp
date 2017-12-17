#include "RenderMInstanced.h"




RenderMInstanced::RenderMInstanced(IRenderable * a_InstancedRenderable, const int a_DrawAmount) {
	m_Renderable = a_InstancedRenderable;
	m_DrawAmount = a_DrawAmount;
}

RenderMInstanced::~RenderMInstanced() {
}

void RenderMInstanced::draw() {
	m_Renderable->drawInstance(m_DrawAmount);
}

void RenderMInstanced::drawInstance(unsigned int a_Amount) {
	m_Renderable->drawInstance(a_Amount);
}
