#pragma once
#include "DLLBuild.h"

#include <queue>
#include <windows.h>

#include "Multithreading/MultithreadManager.h"

class DLL_BUILD MtmThread {
	friend MultithreadManager;
public:
	MtmThread();
	~MtmThread();

	void queueMethod(void* a_Class, MtmMethodQueue a_Method);
	void queueMethod(MtmMethodQueue a_Method) {
		queueMethod(nullptr, a_Method);
	}
	void queueMethod(MtmQueueObj a_Obj) {
		queueMethod(a_Obj.m_Owner, a_Obj.m_Method);
	}

	MtmQueueObj getCurrentMethod();
	void queueCurrentMethod() {
		queueMethod(getCurrentMethod());
	}

	void waitForThread();
protected:
	void startThread();
	void closeThread();

private:
	std::queue<MtmQueueObj> m_JobQueue;
	DWORD m_ThreadID;
	HANDLE m_Thread;
	HANDLE m_Semaphore;
	HANDLE m_IsFree;
	bool m_IsRunning = true;

	static DWORD WINAPI threadRunStatic(LPVOID a_ThreadRef);
	void threadRun();

};

