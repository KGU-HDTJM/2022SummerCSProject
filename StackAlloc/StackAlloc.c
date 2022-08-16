#include <stdlib.h>

#define STACK_ALLOCATION_EXPORTS
#include "StackAlloc.h"

pStack_t __cdecl CreateStack(size_t size/*, heap* memory*/)
{
	pStack_t stack;

	size = GetAlignedSize(size);
	stack = malloc(sizeof(Stack_t) + size + 1);
	if (!stack) { return NULL; }

	stack->BP = (byte_t*)stack + sizeof(Stack_t) + size;
	stack->SP = stack->BP;

	return stack;
}

void __cdecl ReleaseStack(pStack_t stack)
{
	free(stack);
}

boolean_t __cdecl StackOF(pStack_t stack)
{
	return (Word_t)((byte_t*)stack + sizeof(Stack_t)) > (Word_t)stack->SP;	
}

boolean_t __cdecl StackUF(pStack_t stack)
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
