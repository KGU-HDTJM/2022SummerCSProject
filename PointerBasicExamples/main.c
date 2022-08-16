#include <stdio.h>
#include <stdlib.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "StackAlloc.h"
#include "LinearAlgebra.h"

void VoidRef(void* ptr)
{
	printf("%f\n", *(float**)ptr);
}

void PushMatrix(pStack_t stack, Matrix4_t* mat4)
{
	Matrix4_t* mat = SubSP(stack, sizeof(Matrix4_t));
	*mat = *mat4;
}

Matrix4_t* PopMatrix(pStack_t stack)
{
	Matrix4_t* result = stack->SP;
	AddSP(stack, sizeof(Matrix4_t));
}

int main(void)
{
	pStack_t stack = CreateStack(0x100);

	int a = 3;
	Push(stack, &a);
	printf("%d\n", *(int*)Pop(stack));
	*PopMatrix(stack);
	ReleaseStack(stack);
}
