﻿#include "ThreadPool.h"
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
	Stop();
	WaitForMultipleObjects(DEF_THREAD_NUM, threads, true, INFINITE);
	for (int i = 0; i < DEF_THREAD_NUM; i++)
	{
		CloseHandle(threads[i]);
	}
	CloseHandle(mtx);
}

DWORD WINAPI ThreadPool::ThreadStartRoutine(LPVOID lpParam) {
	ThreadPool* pObj = (ThreadPool*)lpParam;
	return pObj->AwaitLoop();
}

DWORD WINAPI ThreadPool::AwaitLoop() {
	while (isRunning) {
		void (*task)(LPVOID taskParam) = NULL;
		LPVOID taskParam = NULL;
		WaitForSingleObject(mtx, INFINITE);
		{
			if (!taskQueue.empty())
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
			stopCondition = taskQueue.size() == 0 && refCount == 0;
		}
		ReleaseMutex(mtx);
	}
}

void ThreadPool::Stop() {
	bool stopCondition = false;
	while (!stopCondition) {
		WaitForSingleObject(mtx, INFINITE);
		{
			stopCondition = taskQueue.size() == 0 && refCount == 0;
			if (stopCondition)
				isRunning = false;
		}
		ReleaseMutex(mtx);
	}
}
