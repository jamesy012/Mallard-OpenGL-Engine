#include "RenderMList.h"


void RenderMList::draw() {
	for (size_t i = 0; i < m_RenderList.size(); i++) {
		m_RenderList[i]->draw();
	}
}

void RenderMList::drawInstance(unsigned int a_Amount) {
	_ASSERT_EXPR(false, L"RenderMList has no Instanced Draw");
}

void RenderMList::addObject(IRenderable * a_Renderable) {
	m_RenderList.push_back(a_Renderable);
}
