#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool() {
	refCount = 0;
	isRunning = true;
	mtx = CreateMutex(NULL, false, NULL);

	for (int i = 0; i < DEF_THREAD_NUM;i++) {
		HANDLE hThread = CreateThread(
			NULL, 0, ThreadStartRoutine, (void*)this, 0, 0);
		threads[i] = hThread;
	}
}

ThreadPool::~ThreadPool() {
	
}

static DWORD WINAPI ThreadStartRoutine(LPVOID lpParam) {
	ThreadPool* pObj = (ThreadPool*)lpParam;
	return pObj->AwaitLoop;
}

void ThreadPool::AwaitLoop() {
	void (*task)(LPVOID taskParam) = NULL;
	LPVOID taskParam = NULL;

	while (isRunning) {
		WaitForSingleObject(mtx, INFINITE);
		{
			if (taskQueue.size() != 0)
			{
				task = this->taskQueue.front();
				taskParam = this->taskParams.front();

				this->taskQueue.pop_front();
				this->taskParams.pop_front();
			}
		}
		ReleaseMutex(mtx);

		if (task != NULL) {
			InterlockedIncrement(&refCount);
			task(taskParam);
			InterlockedDecrement(&refCount);
		}
	}
	ExitThread(0);
}

void ThreadPool::EnqueueTask(void (*task)(LPVOID lpParam), LPVOID lpParam) { 
	WaitForSingleObject(mtx, INFINITE);
	{

	}
	ReleaseMutex(mtx);
}
