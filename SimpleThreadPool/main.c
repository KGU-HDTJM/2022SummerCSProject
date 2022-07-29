#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include "StackAlloc.h"

/*
* 15장. 쓰레드 풀의 구현
* https://youtu.be/Kx6Mb9dV354
* 
 */
#define WORK_MAX 32
#define THREAD_MAX 16

typedef void(*Work_t)(void);

typedef struct
{
	HANDLE hThread;
	DWORD idThread;
} WorkerThread_t;

struct ThreadPool_s
{
	Work_t WorkList[WORK_MAX];

	WorkerThread_t WorkerThreadList[THREAD_MAX];
	HANDLE WorkerEventList[THREAD_MAX];

	DWORD CurrentWorkIndex;
	DWORD LastestWorkIndex;

	DWORD ThreadCount;
} ThreadPool;

DWORD AddWorkToPool(Work_t work);
Work_t GetWorkFromPool(void);
DWORD MakeThreadToPool(DWORD threadNum);
void WorkerThreadFunciton(void* pParam);

static HANDLE mutex = NULL;

void InitMutex(void)
{
	mutex = CreateMutex(NULL, FALSE, NULL);
}

void DeInitMutex(void)
{
	BOOL ret = CloseHandle(mutex);
}

void AcquireMutex(void)
{
	DWORD ret = WaitForSingleObject(mutex, INFINITE);
}