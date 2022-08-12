#pragma once

#include "HDTJMType.h"
#include "HDTJMDef.h"

#if !defined(STACKALLOC_H)
#define STACKALLOC_H

#if defined(STACK_ALLOCATION_EXPORTS)
#define STACK_API __declspec(dllexport)
#else
#define STACK_API __declspec(dllimport)
#endif

typedef struct
{
	char* SP, * BP;
}Stack_t, * pStack_t;

extern STACK_API pStack_t __cdecl CreateStack(size_t size);
extern STACK_API void __cdecl ReleaseStack(pStack_t stack);
extern STACK_API boolean_t __cdecl StackOF(pStack_t stack);
extern STACK_API boolean_t __cdecl StackUF(pStack_t stack);
extern STACK_API void* __cdecl SubSP(pStack_t stack, size_t size);
extern STACK_API void __cdecl AddSP(pStack_t stack, size_t size);
extern STACK_API void __cdecl Push(pStack_t stack, const Word_t* valuePtr);
extern STACK_API Word_t* __cdecl Pop(pStack_t stack);

#endif
