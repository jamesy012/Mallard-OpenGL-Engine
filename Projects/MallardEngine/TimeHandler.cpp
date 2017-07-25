#include "TimeHandler.h"

#include <GLFW\glfw3.h>

#include <stdio.h>
#include <math.h>

//STATIC VARIABLES
static float m_DeltaTime;
static float m_UnscaledDeltaTime;
static float m_PreviousTime;
static float m_CurrentTime;
static unsigned int m_CurrentFrame;

static float m_DeltaTimeScale = 1.0f;
static float m_MaxDeltaTime = 0.5f;

unsigned int TimeHandler::getCurrentFrameNumber() {
	return m_CurrentFrame;
}

float TimeHandler::getCurrentTime() {
	return (float)m_CurrentTime;
}

float TimeHandler::getPreviousTime() {
	return m_PreviousTime;
}

float TimeHandler::getDeltaTime() {
	return m_DeltaTime;
}

float TimeHandler::getUnscaledDeltaTime() {
	return m_UnscaledDeltaTime;
}

float TimeHandler::getDeltaTimeScale() {
	return m_DeltaTimeScale;
}

void TimeHandler::setDeltaTimeScale(float a_Scale) {
	m_DeltaTimeScale = a_Scale;
}

void TimeHandler::setMaxDeltaTime(float a_MaxDt) {
	m_MaxDeltaTime = a_MaxDt;
}

void TimeHandler::update() {
	m_CurrentFrame++;

	float time = (float)glfwGetTime();
	m_UnscaledDeltaTime = time - m_PreviousTime;
	m_PreviousTime = time;

	m_UnscaledDeltaTime = m_UnscaledDeltaTime > m_MaxDeltaTime ? m_MaxDeltaTime : m_UnscaledDeltaTime;
	//printf("deltaTime: %f\n", m_UnscaledDeltaTime);

	m_CurrentTime += m_UnscaledDeltaTime;

	m_DeltaTime = m_UnscaledDeltaTime * m_DeltaTimeScale;

	
}

