#pragma once
#include "DLLBuild.h"

//#include <thread>
//#include <mutex>
#include <queue>

#include <windows.h>

class Application;
class DLL_BUILD MultithreadManager {
friend Application;
public:
	typedef void(*MethodQueue) ();

	MultithreadManager();
	~MultithreadManager();

protected:
	void startThread();
	void closeThread();

	void queueMethod(MethodQueue a_Method);

private:
	std::queue<MethodQueue> m_JobQueue;

	//std::thread* m_OpenGLThread = nullptr;
	//std::mutex m_GLThreadMutex;
	DWORD threadID;
	HANDLE thread;

	HANDLE semaphore;
	bool threadRunning = false;

	static DWORD WINAPI threadRun(LPVOID a_MtM);
};

