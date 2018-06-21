#include "MultithreadManager.h"

#include <iostream>

#include "Multithreading/MtmThread.h"

static bool m_Running = true;

MtmThread* MultithreadManager::m_OpenGLThread;
MtmThread* MultithreadManager::m_LoadingThread;

MultithreadManager::MultithreadManager() {
	m_OpenGLThread = startThread();
	m_LoadingThread = startThread();
}


MultithreadManager::~MultithreadManager() {
	for (int i = 0; i < m_Threads.size();i++) {
		m_Threads[i]->closeThread();
		delete m_Threads[i];
	}
}

MtmThread * MultithreadManager::startThread() {
	MtmThread* thread = new MtmThread();
	thread->startThread();
	m_Threads.push_back(thread);
	return thread;
}

void MultithreadManager::closeThread(MtmThread * a_Thread) {
	a_Thread->closeThread();
	m_Threads.erase(std::find(m_Threads.begin(), m_Threads.end(), a_Thread));
	delete a_Thread;
}

