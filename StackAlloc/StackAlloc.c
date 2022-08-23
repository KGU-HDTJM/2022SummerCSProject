#define STACK_ALLOCATION_EXPORTS
#include "StackAlloc.h"

pStack_t __fastcall _CreateStack(pHeap_t heap, size_t size)
{
	pStack_t stack;

	size = GetAlignedSize(size);
	stack = _HAlloc(heap, sizeof(Stack_t) + size + 1, False, NULL);
	if (!stack) { return NULL; }

	stack->BP = (byte_t*)stack + sizeof(Stack_t) + size;
	stack->SP = stack->BP;

	return stack;
}

void __fastcall _ReleaseStack(pHeap_t heap, pStack_t stack)
{
	_HFree(heap, stack);
}

boolean_t __fastcall StackOF(pStack_t stack)
{
	return (Word_t)((byte_t*)stack + sizeof(Stack_t)) > (Word_t)stack->SP;	
}

boolean_t __fastcall StackUF(pStack_t stack)
{
	return (Word_t)stack->BP < (Word_t)stack->SP;
}

void* __fastcall SubSP(pStack_t stack, size_t size)
{
	stack->SP -= size;
	return stack->SP;
}

void __fastcall AddSP(pStack_t stack, size_t size)
{
	stack->SP += size;
}

void __fastcall Push(pStack_t stack, const Word_t* valuePtr)
{
	stack->SP -= sizeof(Word_t);
	*(long long*)stack->SP = *valuePtr;
}

Word_t* __fastcall Pop(pStack_t stack)
{
	stack->SP += sizeof(Word_t);
	return (Word_t*)(stack->SP - sizeof(Word_t));
}
