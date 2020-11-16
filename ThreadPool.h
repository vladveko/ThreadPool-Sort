#pragma once

#include <Windows.h>
#include <list>

#define DEF_THREAD_NUM 5

class ThreadPool {

	static DWORD WINAPI ThreadStartRoutine(LPVOID lpParam);

private:
	bool isRunning;
	unsigned int refCount;
	std::list<void (*)(LPVOID lpParam)> taskQueue;
	std::list<LPVOID> taskParams;
	HANDLE threads[DEF_THREAD_NUM] = {};
	HANDLE mtx;

	DWORD WINAPI AwaitLoop();

public:
	ThreadPool();
	~ThreadPool();

	void EnqueueTask(void (*task)(LPVOID lpParam), LPVOID lpParam);
	void WaitAll();
};