#include <stdlib.h>
#include <stdio.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "HDTJMError.h"
#include "LinearAlgebra.h"

#include "Z_ZoneCopy.c"

// canary
#define MEM_ID		0x1d4a11

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

void ClearHeap(pHeap_t heap)
{
	pMemBlock_t block;

	heap->BlockList.Next = heap->BlockList.Prev
		= block = ((byte_t*)heap + sizeof(Heap_t));
	heap->BlockList.User = (void*)heap;
	heap->BlockList.bIsCache = False;
	heap->Rover = block;

	block->Prev = block->Next = &heap->BlockList;

	block->User = NULL;
	block->Size = heap->Size - sizeof(Heap_t);	
}

void HFree(pHeap_t heap, void* ptr)
{
	pMemBlock_t block;
	pMemBlock_t other;

	block = ((byte_t*)ptr - sizeof(MemBlock_t));

	Assert(block->ID != ZONE_ID, "HFree: ");

	if (IsPointer(block->User))
	{
		*block->User = NULL;
	}

	block->User = NULL;
	block->bIsCache = False;
	block->ID = 0;

	other = block->Prev;

	if (!other->User)
	{
		other->Size += block->Size;
		other->Next = block->Next;
		other->Next->Prev = other;

		if (block == MainHeap->Rover)
		{
			MainHeap->Rover = other;
		}
		
		block = other;
	}

	other = block->Next;

	if (!other->User)
	{
		block->Size += other->Size;
		block->Next = other->Next;
		block->Next->Prev = block;

		if (other == MainHeap->Rover)
		{
			MainHeap->Rover = block;
		}
	}
}

#define MINFRAGMENT 128
void* HAlloc(pHeap_t heap, size_t size, boolean_t bCache, void* user)
{
	size_t extra;
	pMemBlock_t start;
	pMemBlock_t rover;
	pMemBlock_t newBlock;
	pMemBlock_t base;

	size = GetAlignedSize(size) + sizeof(MemBlock_t);

	base = heap->Rover;

	if (!base->Prev->User) { base = base->Prev; }

	rover = base;
	start = base->Prev;

	do
	{
		if (rover == start)
		{
			Assert(rover == start, 
				"MyAlloc: failed on allocation of %llu byte\n");
			return NULL;
		}

		if (rover-> User)
		{
			if (rover->bIsCache)
			{
				base = base->Prev;
				HFree(heap, (byte_t*)rover + sizeof(MemBlock_t));
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

	base->ID = MEM_ID;
	return (byte_t*)base + sizeof(MemBlock_t);
}


void DumpHeap(pHeap_t heap)
{
	MemBlock_t* block;
	printf("heap size: %llu\tlocation: %p\n", heap->Size, heap);

	for (block = heap->BlockList.Next; ; block = block->Next)
	{
		printf("block: %p\tsize:%8llu\tuser: %p\tbIsCache: %c",
			block, block->Size, block->User, 
			0x46 + 0xe * block->bIsCache);
		
		if (block->Next == &heap->BlockList)
		{
			break;
		}

		if ((byte_t*)block + block->Size != (byte_t*)block->Next)
		{
			printf("Error: block size does not touch the next block\n");
		}
		if (block->Next->Prev != block)
		{
			printf("Error: next block doesn't have proper black link\n");
		}
		if (!block->User && !block->Next->User)
		{
			printf("Error: two consecutive free blocks\n");
		}
	}
}



void ReleaseMainHeap(void)
{
	ClearHeap(MainHeap);
	free(MainHeap);
}

int main(void)
{
	pHeap_t mainMemory = InitMem(4 * 0x400 * 0x400);
	 
}