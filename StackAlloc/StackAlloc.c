#include <stdlib.h>

#define STACK_ALLOCATION_EXPORTS

#include "StackAlloc.h"

pStack_t __cdecl CreateStack(size_t size)
{
	pStack_t stack = malloc(sizeof(Stack_t));
	size = GetAlignedSize(size);
	if (!stack) { return NULL; }
	stack->memBlock = malloc(size + 1);
	if (!stack->memBlock)
	{
		free(stack);
		return NULL;
	}
	stack->BP = (char*)stack->memBlock + size;
	stack->SP = stack->BP;

	return stack;
}

void __cdecl ReleaseStack(pStack_t stack)
{
	free(stack->memBlock);
	free(stack);
}

bool __cdecl StackOF(pStack_t stack)
{
	return (Word_t)stack->memBlock < (Word_t)stack->SP;
}

bool __cdecl StackUF(pStack_t stack)
{
	return (Word_t)stack->BP < (Word_t)stack->SP;
}

void* __cdecl SubSP(pStack_t stack, size_t size)
{
	stack->SP -= size;
	return stack->SP;
}

void __cdecl AddSP(pStack_t stack, size_t size)
{
	stack->SP += size;
}

void __cdecl Push(pStack_t stack, const Word_t* valuePtr)
{
	stack->SP -= sizeof(Word_t);
	*(long long*)stack->SP = *valuePtr;
}

Word_t* __cdecl Pop(pStack_t stack)
{
	stack->SP += sizeof(Word_t);
	return (Word_t*)(stack->SP - sizeof(Word_t));
}
