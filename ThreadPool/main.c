#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <process.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "HDTJMError.h"
#include "HeapAlloc.h"
#include "StackAlloc.h"

#include "LinearAlgebra.h"

int main(void)
{
	pHeap_t mainMemory = InitMem(1024Ui64 * 1024 * 4);

	DumpHeap(NULL);
	pStack_t stack = CreateStack(mainMemory, 6Ui64 * 1024);
	DumpHeap(NULL);

	ReleaseStack(mainMemory, stack);
	DumpHeap(NULL);
	printf("%llu\n", GetFreeMemSize(mainMemory));

	ReleaseMainMem();
	return 0;
}