#include "Logging.h"

#include <GL\glew.h>

#include <chrono>
#include <stack>

static bool m_AllowRenderedVerticesAdditions = true;
static unsigned int m_NumOfRenderedVertices = 0;

static int m_GpuDebugGroups = 0;

typedef std::chrono::steady_clock ChronoTimeType;
typedef std::chrono::time_point<ChronoTimeType> ChronoTime;

struct TimerObject {
	ChronoTime m_Start;
	ChronoTime m_End;
	std::chrono::duration<float> m_TotalTime;
	std::string m_Desc;
	unsigned int m_Index;
	bool m_CurrentlyTimed;
	bool m_HasLapped;
	unsigned int m_LapNumber;
};

static std::stack<TimerObject*> m_QuickTimeStack;

//returns the current time for the
static ChronoTime getCurrentTime() {
	return ChronoTimeType::now();
}

void Logging::newFrame() {
	m_NumOfRenderedVertices = 0;
}

void Logging::objectRenderedAllowAdditions(bool a_AllowAdditions) {
	m_AllowRenderedVerticesAdditions = a_AllowAdditions;
}

void Logging::objectRendered(unsigned int a_NumOfVertices) {
	if (!m_AllowRenderedVerticesAdditions) {
		return;
	}
	m_NumOfRenderedVertices += a_NumOfVertices;
}

unsigned int Logging::getNumVerticesRendered() {
	return m_NumOfRenderedVertices;
}

void Logging::quickTimePush(std::string a_Name, bool a_DisplayToConsole) {
	TimerObject* timer = new TimerObject();
	timer->m_Desc = a_Name;
	m_QuickTimeStack.push(timer);

	if (a_DisplayToConsole) {
		printf("DebugTimerPush: Name: %s\n", timer->m_Desc.c_str());
	}

	timer->m_Start = getCurrentTime();
}

void Logging::quickTimePop(bool a_DisplayToConsole) {
	ChronoTime endTime = getCurrentTime();

	_ASSERT_EXPR(m_QuickTimeStack.size() != 0, L"Cant pop, there was no pushed time");

	TimerObject* timer = m_QuickTimeStack.top();
	m_QuickTimeStack.pop();

	if (a_DisplayToConsole) {
		std::chrono::duration<float> duration = endTime - timer->m_Start;

		printf("DebugTimerPop: (%i)\tName: %s, Time: (%f)ms\n", m_QuickTimeStack.size() , timer->m_Desc.c_str(), duration.count() * 1000);
	}

	delete timer;
}

void Logging::quickGpuDebugGroupPush(std::string a_Text) {
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, m_GpuDebugGroups++, -1, a_Text.c_str());
}

void Logging::quickGpuDebugGroupPop() {
	glPopDebugGroup();
	m_GpuDebugGroups--;
}
