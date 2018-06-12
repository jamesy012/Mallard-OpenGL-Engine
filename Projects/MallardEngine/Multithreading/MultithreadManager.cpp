#include "MultithreadManager.h"

#include <iostream>

static bool m_Running = true;

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

	//mutex = CreateMutex(
	//	NULL,              // default security attributes
	//	FALSE,             // initially not owned
	//	NULL);             // unnamed mutex
	semaphore = CreateSemaphore(
		NULL,           // default security attributes
		1,  // initial count
		1,  // maximum count
		NULL);          // unnamed semaphore

	if (semaphore == NULL) {
		printf("CreateSemaphore error: %d\n", GetLastError());
	}
}

void MultithreadManager::closeThread() {
	if (m_JobQueue.size() != 0) {
		std::cout << "Running the rest of the queued methods! " << m_JobQueue.size() + (threadRunning?1:0) << " left" << std::endl;

	}
	m_Running = false;

	ReleaseSemaphore(semaphore, 1, NULL);

	WaitForSingleObject(thread, INFINITE);

	CloseHandle(thread);
	CloseHandle(semaphore);

}

void MultithreadManager::queueMethod(MethodQueue a_Method) {
	std::cout << "adding Multi METHOD" << std::endl;

	m_JobQueue.push(a_Method);
	ReleaseSemaphore(semaphore, 1, NULL);
	
}

DWORD WINAPI MultithreadManager::threadRun(LPVOID a_MtM) {
	std::cout << "MULTI thread START" << std::endl;
	MultithreadManager* mtm = (MultithreadManager*)a_MtM;
	while (true) {
		//sleep thread until we receive something to do
		DWORD dwWaitResult = WaitForSingleObject(
			mtm->semaphore,    // handle to mutex
			INFINITE);  // no time-out interval

		//if we have no jobs left and we are trying to close the thread then exit
		if (!m_Running && mtm->m_JobQueue.size() == 0) {
			break;
		}

		std::cout << "MULTI thread -- RUNNING METHOD" << std::endl;

		//get method
		MethodQueue method = mtm->m_JobQueue.front();
		mtm->m_JobQueue.pop();

		//run method
		{
			mtm->threadRunning = true;
			method();
			mtm->threadRunning = false;
		}

		//if there is a job left then release
		//else if we have finished the program, break the loop
		if (mtm->m_JobQueue.size() != 0) {
			ReleaseSemaphore(mtm->semaphore, 1, NULL);
		} else if (!m_Running) {
			break;
		}
	}
	return 0;
}
