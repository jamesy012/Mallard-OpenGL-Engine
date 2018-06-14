#pragma once
#include "DLLBuild.h"

//#include <thread>
//#include <mutex>
#include <queue>

#include <windows.h>

class Application;
class DLL_BUILD MultithreadManager {
friend Application;

typedef void(*MethodQueue) (void*);
public:
	struct QueueObj {
		MethodQueue mq;
		void* Owner;
	};

	MultithreadManager();
	~MultithreadManager();

	static void queueMethodThread2(MethodQueue a_Method);
	static void queueMethod(void* a_Class, MethodQueue a_Method);
	static void queueMethod(MethodQueue a_Method) {
		queueMethod(nullptr, a_Method);
	}
	static void queueMethod(QueueObj a_Obj) {
		queueMethod(a_Obj.Owner, a_Obj.mq);
	}
	static void queueCurrentMethod();
	static QueueObj getCurrentMethod();

	void waitForThread();
protected:
	void startThread();
	void closeThread();


private:

	static std::queue<QueueObj> m_JobQueue;

	//std::thread* m_OpenGLThread = nullptr;
	//std::mutex m_GLThreadMutex;
	DWORD threadID;
	HANDLE thread;

	DWORD threadID2;
	HANDLE thread2;
	static HANDLE semaphore2;
	static QueueObj thread2Job;

	static HANDLE semaphore;
	static HANDLE m_isFree;
	bool threadRunning = false;

	static DWORD WINAPI threadRun(LPVOID a_MtM);
	static DWORD WINAPI threadRun2(LPVOID a_MtM);
};

