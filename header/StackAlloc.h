#pragma once
#include <stdbool.h>

#if defined STACK_ALLOCATION_EXPORTS
#define STACK_API __declspec(dllexport)
#else
#define STACK_API __declspec(dllimport)
#endif

#if defined (_WIN64)
// x86_64
typedef long long Word_t;
#define GetAlignedSize(size) ((size + 7) & ~7)
#else
// x86
typedef int Word_t;
#define GetAlignedSize(size) ((size + 3) & ~3)
#endif

typedef struct _STACK
{
	char* SP,* BP;
	void* memBlock;
}Stack_t, * pStack_t;

extern STACK_API pStack_t __cdecl CreateStack(size_t size);
extern STACK_API void __cdecl ReleaseStack(pStack_t stack);
extern STACK_API bool __cdecl StackOF(pStack_t stack);
extern STACK_API bool __cdecl StackUF(pStack_t stack);
extern STACK_API void* __cdecl SubSP(pStack_t stack, size_t size);
extern STACK_API void __cdecl AddSP(pStack_t stack, size_t size);
extern STACK_API void __cdecl Push(pStack_t stack, const Word_t* valuePtr);
extern STACK_API Word_t* __cdecl Pop(pStack_t stack);
