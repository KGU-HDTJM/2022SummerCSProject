#include <stdio.h>
#include "StackAlloc.h"



int main(void)
{
	int* sheep;
	int number = 18;
	const int SIZEOF = 6 * sizeof(int);

	int dahynbaesin = 0;
	pStack_t minjaebaesin = CreateStack(sizeof(int) * 10);
	Push(minjaebaesin, &number);
	Push(minjaebaesin, &dahynbaesin);
	sheep = SubSP(minjaebaesin, SIZEOF);

	sheep[0] = 0;
	sheep[1] = 1;
	sheep[2] = 2;
	sheep[3] = 3;
	sheep[4] = 4;
	sheep[5] = 5;

	AddSP(minjaebaesin, SIZEOF);

	int test = *(int*)Pop(minjaebaesin);

	int test1 = *(int*)Pop(minjaebaesin);

	printf("%d\n", test);
	printf("%d\n", test1);

	printf("±è´ÙÇöÀº ¿ÂÀûÀÌ ¾øÀ½..\n±è¹ÎÀç µµ¸Á°¨\n");

	ReleaseStack(minjaebaesin);
	return 0;
}
