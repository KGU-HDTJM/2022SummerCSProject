#include <stdio.h>
#include <intrin.h>

#include "HDTJMThread.h"

void __fastcall SpinlockAcquire(volatile boolean_t* bLock)
{
	while (1)
	{
		while (*bLock);
		// if (TAS(lock))
		{
			break;
		}
	}
}

void __fastcall SpinlockRelease(volatile boolean_t* bLock)
{
	//
}

int main(void)
{
	boolean_t bLock = 0;
	for (size_t i = 0; i < 2; i++)
	{
		printf("%llu: ", i);
		if (!TASBoolean(&bLock))
		{
			printf("Locked\n");
		}
		else
		{
			printf("Wait\n");
		}
	}
}
