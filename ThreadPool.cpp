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

DWORD WINAPI ThreadPool::ThreadStartRoutine(LPVOID lpParam) {
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

void ThreadPool::EnqueueTask(void (*task)(LPVOID lpParam), LPVOID taskParam) { 
	WaitForSingleObject(mtx, INFINITE);
	{
		if (isRunning) {
			this->taskQueue.push_back(task);
			this->taskParams.push_back(taskParam);
		}
	}
	ReleaseMutex(mtx);
}

void ThreadPool::WaitAll() {
	bool stopCondition = false;
	while (!stopCondition) {
		WaitForSingleObject(mtx, INFINITE);
		{
			stopCondition = taskQueue.size == 0 && refCount == 0;
			if (stopCondition)
				isRunning = false;
		}
		ReleaseMutex(mtx);
	}
}
