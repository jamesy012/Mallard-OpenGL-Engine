#include "MtmThread.h"

#include <iostream>

MtmThread::MtmThread() {
}


MtmThread::~MtmThread() {
}

void MtmThread::queueMethod(void * a_Class, MtmMethodQueue a_Method) {
	m_JobQueue.push({ a_Method, a_Class });
	ReleaseSemaphore(m_Semaphore, 1, NULL);
}

MtmQueueObj MtmThread::getCurrentMethod() {
	if (m_JobQueue.size() == 0) {
		return {};
	}
	return m_JobQueue.front();
}

void MtmThread::waitForThread() {
	//this could be better
	while (m_JobQueue.size() != 0) {
		DWORD dwWaitResult = WaitForSingleObject(
			m_IsFree,    // handle to mutex
			INFINITE);  // no time-out interval
		ReleaseSemaphore(m_Semaphore, 1, NULL);
	}
	ReleaseSemaphore(m_IsFree, 1, NULL);
}

void MtmThread::startThread() {
	m_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MtmThread::threadRunStatic, this, 0, &m_ThreadID);
	if (m_Thread == NULL) {
		printf("CreateThread error: %d\n", GetLastError());
	}

	m_Semaphore = CreateSemaphore(
		NULL,           // default security attributes
		0,  // initial count
		1,  // maximum count
		NULL);          // unnamed semaphore
	m_IsFree = CreateSemaphore(
		NULL,           // default security attributes
		0,  // initial count
		1,  // maximum count
		NULL);          // unnamed semaphore

	if (m_Semaphore == NULL || m_IsFree == NULL) {
		printf("CreateSemaphore error: %d\n", GetLastError());
	}
}

void MtmThread::closeThread() {
	m_IsRunning = false;
	if (m_JobQueue.size() != 0) {
		std::cout << "Running the rest of the queued methods! " << m_JobQueue.size() << " left" << std::endl;
		waitForThread();
	}

	ReleaseSemaphore(m_Semaphore, 1, NULL);
	ReleaseSemaphore(m_IsFree, 1, NULL);

	WaitForSingleObject(m_Thread, INFINITE);

	CloseHandle(m_Thread);
	CloseHandle(m_Semaphore);
	CloseHandle(m_IsFree);
}

DWORD MtmThread::threadRunStatic(LPVOID a_ThreadRef) {
	std::cout << "MULTI thread START" << std::endl;
	MtmThread* mtm = (MtmThread*)a_ThreadRef;
	mtm->threadRun();
	return 0;
}

void MtmThread::threadRun() {
	while (true) {
		//sleep thread until we receive something to do
		DWORD dwWaitResult = WaitForSingleObject(
			m_Semaphore,    // handle to object
			INFINITE);  // no time-out interval

						//if we have no jobs left and we are trying to close the thread then exit
		if (!m_IsRunning && m_JobQueue.size() == 0) {
			break;
		}
		if (m_JobQueue.size() == 0) {
			ReleaseSemaphore(m_IsFree, 1, NULL);
			continue;
		}

		//std::cout << "MULTI thread -- RUNNING METHOD" << std::endl;

		//get method
		MtmQueueObj method = m_JobQueue.front();

		//run method
		method.m_Method(method.m_Owner);

		m_JobQueue.pop();

		//else if we have finished the program, break the loop
		if (m_JobQueue.size() == 0 && !m_IsRunning) {
			break;
		}
		ReleaseSemaphore(m_Semaphore, 1, NULL);

	}
}


