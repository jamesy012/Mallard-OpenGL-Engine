#include "RenderingList.h"

#include "Renderable.h"
#include "RenderingObject.h"

RenderingList::RenderingList() {
}

RenderingList::~RenderingList() {
	for (uint i = 0; i < m_TempObjects.size(); i++) {
		delete m_TempObjects[i]->m_Renderer;
		delete m_TempObjects[i];
	}
}

void RenderingList::draw() {
	for (uint i = 0; i < m_Objects.size(); i++) {
		m_Objects[i]->m_Renderer->fullDraw();
	}
}

void RenderingList::CreateTempRenderingHolder(Object * a_Object) {
	RenderingObject* ro = new RenderingObject();
	RenderingHolder* rh = new RenderingHolder();

	ro->m_Object = a_Object;
	rh->m_Renderer = ro;
	//todo: Shader

	m_TempObjects.push_back(rh);
	m_Objects.push_back(rh);
}
