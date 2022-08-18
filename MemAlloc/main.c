#include <stdlib.h>
#include <stdio.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"

#include "Z_ZoneCopy.c"

// canary
#define ID_FOR_ALL		0x1d4a11

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

// The size of the buffer must be an aligned size.
static pHeap_t MainHeap = NULL;

pHeap_t InitSubMem(void* buf, size_t size)
{
	pHeap_t result;
	pMemBlock_t block;
	size = GetAlignedSize(size);
	if (buf == NULL) { return NULL; }
	result = buf;
	result->Size = size;

	result->BlockList.Next = result->BlockList.Prev
		= block = ((byte_t*)result + sizeof(Heap_t));

	result->BlockList.User = result;
	result->BlockList.bIsCache = False;
	result->Rover = block;

	block->Prev = block->Next = &result->BlockList;

	block->User = NULL;
	block->Size = result->Size - sizeof(Heap_t);

	return result;
}

pHeap_t InitMem(size_t heapSize)
{
	const size_t size = GetAlignedSize(heapSize);

	MainHeap = InitSubMem(malloc(size), size);

	return MainHeap;
}

//void ClearHeap(pHeap_t heap)
//{
//	MemBlock_t* block;
//
//	heap->BlockList.Next = heap->BlockList.Prev
//		= block = ((byte_t*)heap + sizeof(Heap_t));
//	heap->BlockList.User = (void*)heap;
//	heap->BlockList.Tag = MEMTAG_MANUAL;
//	heap->Rover = block;
//
//	block->Prev = block->Next = &heap->BlockList;
//
//	block->User = NULL;
//	block->Size = heap->Size - sizeof(Heap_t);	
//}
#define MINFRAGMENT 64
void* Malloc(pHeap_t heap, size_t size, boolean_t bCache, void* user)
{
	size_t extra;
	MemBlock_t* start;
	MemBlock_t* rover;
	MemBlock_t* newBlock;
	MemBlock_t* base;

	size = GetAlignedSize(size) + sizeof(MemBlock_t);

	base = heap->Rover;

	if (!base->Prev->User) { base = base->Prev; }

	rover = base;
	start = base->Prev;

	do
	{
		if (rover == start)
		{
#if defined(_DEBUG)
			printf("MyAlloc: failed on allocation of %llu byte\n", size);
#endif
			return NULL;
		}
		
		if (rover->User)
		{
			if (rover->bIsCache)
			{
				base = base->Prev;
				// Free((byte_t*)rover + sizeof(MemBlock_t));
				base = base->Next;
				rover = base->Next;
			}
			else
			{
				base = rover = rover->Next;
			}
		}
		else
		{
			rover = rover->Next;
		}
	} while (base->User || base->Size < size);

	extra = base->Size - size;
	if (MINFRAGMENT < extra)
	{
		newBlock = ((byte_t*)base + size);
		newBlock->Size = extra;

		newBlock->User = NULL;
		newBlock->bIsCache = False;
		newBlock->Prev = base;
		newBlock->Next = base->Next;
		newBlock->Next->Prev = newBlock;

		base->Next = newBlock;
		base->Size = size;
	}

	if (user)
	{
		base->User = user;
		*(void**)user = (byte_t*)base + sizeof(MemBlock_t);
	}
	else
	{
		if (bCache)
		{
			printf("Malloc: an owner is required for cache blocks");
			return NULL;
		}
		base->User = (void*)2;
	}
	base->bIsCache = bCache;

	heap->Rover = base->Next;

	base->ID = ID_FOR_ALL;
	return (byte_t*)base + sizeof(MemBlock_t);
}

int main(void)
{
	pHeap_t mainMemory = InitMem(4 * 0x400 * 0x400);
	 
}