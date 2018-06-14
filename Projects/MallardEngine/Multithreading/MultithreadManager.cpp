#include "MultithreadManager.h"

#include <iostream>

static bool m_Running = true;
HANDLE MultithreadManager::semaphore;
HANDLE MultithreadManager::m_isFree;
std::queue<MultithreadManager::QueueObj> MultithreadManager::m_JobQueue;

HANDLE MultithreadManager::semaphore2;
MultithreadManager::QueueObj MultithreadManager::thread2Job;

MultithreadManager::MultithreadManager() {
}


MultithreadManager::~MultithreadManager() {
	m_Running = false;
	//if (m_OpenGLThread != nullptr) {
	//	m_OpenGLThread->join();
	//	m_OpenGLThread = nullptr;
	//}
}

void MultithreadManager::startThread() {
	//m_OpenGLThread = new std::thread(MultithreadManager::threadRun,this);
	thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MultithreadManager::threadRun, this, 0, &threadID);
	if (thread == NULL) {
		printf("CreateThread error: %d\n", GetLastError());
	}

	thread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MultithreadManager::threadRun2, this, 0, &threadID2);
	if (thread2 == NULL) {
		printf("CreateThread error: %d\n", GetLastError());
	}

	//mutex = CreateMutex(
	//	NULL,              // default security attributes
	//	FALSE,             // initially not owned
	//	NULL);             // unnamed mutex
	semaphore = CreateSemaphore(
		NULL,           // default security attributes
		1,  // initial count
		1,  // maximum count
		NULL);          // unnamed semaphore
	m_isFree = CreateSemaphore(
		NULL,           // default security attributes
		1,  // initial count
		1,  // maximum count
		NULL);          // unnamed semaphore

	semaphore2 = CreateSemaphore(
		NULL,           // default security attributes
		0,  // initial count
		1,  // maximum count
		NULL);

	if (semaphore == NULL) {
		printf("CreateSemaphore error: %d\n", GetLastError());
	}
}

void MultithreadManager::closeThread() {
	if (m_JobQueue.size() != 0) {
		std::cout << "Running the rest of the queued methods! " << m_JobQueue.size() + (threadRunning ? 1 : 0) << " left" << std::endl;

	}
	m_Running = false;

	ReleaseSemaphore(semaphore, 1, NULL);
	ReleaseSemaphore(semaphore2, 1, NULL);
	ReleaseSemaphore(m_isFree, 1, NULL);

	WaitForSingleObject(thread, INFINITE);

	CloseHandle(thread);
	CloseHandle(semaphore);
	CloseHandle(semaphore2);
	CloseHandle(m_isFree);

}


void MultithreadManager::queueMethodThread2(MethodQueue a_Method) {
	thread2Job = { a_Method,nullptr };
	ReleaseSemaphore(semaphore2, 1, NULL);
}

void MultithreadManager::queueMethod(void* a_Class, MethodQueue a_Method) {
	//std::cout << "adding Multi METHOD" << std::endl;

	m_JobQueue.push({ a_Method, a_Class });
	ReleaseSemaphore(semaphore, 1, NULL);

	DWORD dwWaitResult = WaitForSingleObject(
		m_isFree,    // handle to mutex
		INFINITE);  // no time-out interval

}

void MultithreadManager::queueCurrentMethod() {
	QueueObj method = m_JobQueue.front();
	m_JobQueue.push({ method.mq, method.Owner });
}

MultithreadManager::QueueObj MultithreadManager::getCurrentMethod() {
	return m_JobQueue.front();
}

void MultithreadManager::waitForThread() {
	//this could be better
	do {
		DWORD dwWaitResult = WaitForSingleObject(
			m_isFree,    // handle to mutex
			100);  // no time-out interval
		ReleaseSemaphore(semaphore, 1, NULL);
	} while (m_JobQueue.size() != 0);
	ReleaseSemaphore(m_isFree, 1, NULL);
}

DWORD WINAPI MultithreadManager::threadRun(LPVOID a_MtM) {
	std::cout << "MULTI thread START" << std::endl;
	MultithreadManager* mtm = (MultithreadManager*)a_MtM;
	while (true) {
		//sleep thread until we receive something to do
		DWORD dwWaitResult = WaitForSingleObject(
			mtm->semaphore,    // handle to object
			INFINITE);  // no time-out interval

		//if we have no jobs left and we are trying to close the thread then exit
		if (!m_Running && mtm->m_JobQueue.size() == 0) {
			break;
		}
		if (mtm->m_JobQueue.size() == 0) {
			ReleaseSemaphore(mtm->m_isFree, 1, NULL);
			continue;
		}

		//std::cout << "MULTI thread -- RUNNING METHOD" << std::endl;

		//get method
		QueueObj method = mtm->m_JobQueue.front();

		//run method
		{
			mtm->threadRunning = true;
			method.mq(method.Owner);
			mtm->threadRunning = false;
		}

		mtm->m_JobQueue.pop();

		//else if we have finished the program, break the loop
		if (mtm->m_JobQueue.size() == 0 && !m_Running) {
			break;
		}
		ReleaseSemaphore(mtm->semaphore, 1, NULL);

	}
	return 0;
}

DWORD MultithreadManager::threadRun2(LPVOID a_MtM) {
	MultithreadManager* mtm = (MultithreadManager*)a_MtM;
	while (true) {
		DWORD dwWaitResult = WaitForSingleObject(
			mtm->semaphore2,    // handle to mutex
			INFINITE);  // no time-out interval

		if (!m_Running) {
			break;
		}

		thread2Job.mq(thread2Job.Owner);

	}
	return 0;
}
