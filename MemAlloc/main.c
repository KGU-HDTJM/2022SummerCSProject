#include <stdlib.h>
#include <stdio.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"

#include "Z_ZoneCopy.c"

// canary
#define ID_FOR_ALL		0x1d4a11

#define MEMTAG_STATIC	0x1
#define MEMTAG_CACHE	0x80

typedef struct MemBlock_s
{
	size_t	Size;
	void** User;
	int		Tag;
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
static pHeap_t MainHeap;

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
	result->BlockList.Tag = MEMTAG_STATIC;
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
	MemBlock_t* block;
}


int main(void)
{
	pHeap_t mainMemory = InitMem(4 * 0x400 * 0x400);

}