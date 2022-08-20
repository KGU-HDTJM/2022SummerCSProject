#pragma once
#include "HDTJMType.h"

#if defined(HEAP_ALLOCATION_EXPORTS)
#define HEAP_API __declspec(dllexport)
#else
#define HEAP_API __declspec(dllimport)
#endif

#if !defined(HEAPALLOC_H)
#define HEAPALLOC_H
typedef struct MemBlock_s
{
	size_t	Size;
	void** User;
	boolean_t bIsCache;
	int		ID; // Canary Value
	struct MemBlock_s* Next, * Prev;
} MemBlock_t, * pMemBlock_t;

typedef struct
{
	size_t Size;
	MemBlock_t BlockList;
	MemBlock_t* Rover;
} Heap_t, * pHeap_t;

extern HEAP_API pHeap_t __cdecl InitSubMem(void* buf, size_t size);
extern HEAP_API pHeap_t __cdecl InitMem(size_t heapSize);
extern HEAP_API void __cdecl ClearHeap(pHeap_t heap);
extern HEAP_API void __cdecl HFree(pHeap_t heap, void* ptr);
extern HEAP_API void* __cdecl HAlloc(pHeap_t heap, size_t size, boolean_t bCache, void* user);
extern HEAP_API void __cdecl DumpHeap(pHeap_t heapOrNULL);
extern HEAP_API void __cdecl DumpHeapFile(FILE* fp, pHeap_t heapOrNULL);
extern HEAP_API void __cdecl CheckHeap(pHeap_t heapOrNULL);
extern HEAP_API void __cdecl ReleaseMainMem(void);
extern HEAP_API size_t __cdecl GetFreeSpace(pHeap_t heap);
#endif