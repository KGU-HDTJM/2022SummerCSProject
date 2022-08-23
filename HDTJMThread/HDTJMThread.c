#include <stdio.h>
#include <intrin.h>

#define THREAD_API_EXPORTS
#include "HDTJMThread.h"

void __fastcall SpinlockAcquire(volatile boolean_t* bLock)
{
	for (;;)
	{
		while (*bLock);
		if (!TASBoolean(bLock))
		{
			break;
		}
	}
}

void __fastcall SpinlockRelease(volatile boolean_t* bLock)
{
	*bLock = False;
}

//int main(void)
//{
//	boolean_t bLock = 0;
//	for (size_t i = 0; i < 2; i++)
//	{
//		printf("%llu: ", i);
//		if (!TASBoolean(&bLock))
//		{
//			printf("Get lock Acquire\n");
//		}
//		else
//		{
//			printf("Wait..\n");
//		}
//	}
//}
