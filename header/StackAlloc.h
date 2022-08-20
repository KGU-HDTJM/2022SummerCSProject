#pragma once
#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "HeapAlloc.h"

#if defined(STACK_ALLOCATION_EXPORTS)

#define STACK_API __declspec(dllexport)

#else

#define STACK_API __declspec(dllimport)

#endif

#if !defined(STACKALLOC_H)

#define STACKALLOC_H

typedef struct
{
	byte_t* SP, * BP;
}Stack_t, * pStack_t;

extern STACK_API pStack_t __cdecl CreateStack(pHeap_t heap, size_t size);
extern STACK_API void __cdecl ReleaseStack(pHeap_t heap, pStack_t stack);
extern STACK_API boolean_t __cdecl StackOF(pStack_t stack);
extern STACK_API boolean_t __cdecl StackUF(pStack_t stack);
// alloc
extern STACK_API void* __cdecl SubSP(pStack_t stack, size_t size);
// free
extern STACK_API void __cdecl AddSP(pStack_t stack, size_t size);
extern STACK_API void __cdecl Push(pStack_t stack, const Word_t* valuePtr);
extern STACK_API Word_t* __cdecl Pop(pStack_t stack);

#endif
