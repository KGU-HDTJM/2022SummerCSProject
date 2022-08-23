#pragma once
#ifndef HEAPALLOC_H
#define HEAPALLOC_H

#include <stdio.h>
#include "HDTJMType.h"

#if defined(HEAP_ALLOCATION_EXPORTS)
#define HEAP_API __declspec(dllexport)
#else
#define HEAP_API __declspec(dllimport)
#endif

#define CREATE_HEAP_SUCCESS 0

typedef struct MemBlock_s
{
	size_t	Size;
	void** User;
	boolean_t bCache;

	int		ID; // Canary Value
	struct MemBlock_s* Next, * Prev;
} MemBlock_t, * pMemBlock_t;

typedef struct
{
	size_t Size;
	boolean_t bLocked;
	MemBlock_t BlockList;
	MemBlock_t* Rover;
} Heap_t, * pHeap_t;

#define HAlloc(size, bCache, user) _HAlloc(NULL, size, bCache, user)
#define HFree(ptr) _HFree(NULL, ptr);

extern HEAP_API pHeap_t __fastcall InitSubMem(void* buf, size_t size);
extern HEAP_API int __fastcall InitMem(size_t heapSize);
extern HEAP_API void __fastcall ClearHeap(pHeap_t heap);
extern HEAP_API void __fastcall _HFree(pHeap_t heap, void* ptr);
extern HEAP_API void* __fastcall _HAlloc(pHeap_t heap, size_t size, boolean_t bCache, void* user);
extern HEAP_API void __fastcall DumpHeap(pHeap_t heapOrNULL);
extern HEAP_API void __fastcall DumpHeapFile(FILE* fp, pHeap_t heapOrNULL);
extern HEAP_API void __fastcall CheckHeap(pHeap_t heapOrNULL);
extern HEAP_API void __fastcall ReleaseMainMem(void);
extern HEAP_API size_t __fastcall GetFreeMemSize(pHeap_t heapOrNULL);

#endif
