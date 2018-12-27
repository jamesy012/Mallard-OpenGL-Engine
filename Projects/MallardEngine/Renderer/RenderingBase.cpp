#include "RenderingBase.h"

#include "GLDebug.h"
#include "Logging.h"

RenderingBase::RenderingBase() {
}
RenderingBase::RenderingBase(const char* a_Name) : m_RenderableName(a_Name) {
}


RenderingBase::~RenderingBase() {
}

void RenderingBase::preDraw() {
	Logging::quickGpuDebugGroupPush(m_RenderableName);
}

void RenderingBase::postDraw() {
	Logging::quickGpuDebugGroupPop();
}
