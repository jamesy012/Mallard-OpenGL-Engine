#pragma once
#include "DLLBuild.h"

#include <vector>

class Application;
class MtmThread;

typedef void(*MtmMethodQueue) (void*);

struct MtmQueueObj {
	MtmMethodQueue m_Method;
	void* m_Owner;
};

class DLL_BUILD MultithreadManager {
friend Application;

public:
	MultithreadManager();
	~MultithreadManager();

	////static void queueMethodThread2(MtmMethodQueue a_Method);
	//static void queueMethod(const MtmThread* a_Thread, void* a_Class, MtmMethodQueue a_Method);
	//static void queueMethod(const MtmThread* a_Thread, MtmMethodQueue a_Method) {
	//	queueMethod(a_Thread, nullptr, a_Method);
	//}
	//static void queueMethod(const MtmThread* a_Thread, MtmQueueObj a_Obj) {
	//	queueMethod(a_Thread, a_Obj.m_Owner, a_Obj.m_Method);
	//}
	//static MtmQueueObj getCurrentMethod(MtmThread* a_Thread);
	//static void queueCurrentMethod(MtmThread* a_Thread);
	//
	//void waitForThread(MtmThread* a_Thread);
//protected:
	MtmThread* startThread();
	void closeThread(MtmThread* a_Thread);

	static MtmThread* m_OpenGLThread;
	static MtmThread* m_LoadingThread;
private:

	//static std::queue<QueueObj> m_JobQueue;
	std::vector<MtmThread*> m_Threads;

	//std::thread* m_OpenGLThread = nullptr;
	//std::mutex m_GLThreadMutex;
	//DWORD threadID;
	//HANDLE thread;

	//DWORD threadID2;
	//HANDLE thread2;
	//static HANDLE semaphore2;
	//static QueueObj thread2Job;
	//
	//static HANDLE semaphore;
	//static HANDLE m_isFree;
	//bool threadRunning = false;

	//static DWORD WINAPI threadRun(LPVOID a_MtM);
	//static DWORD WINAPI threadRun2(LPVOID a_MtM);

};


