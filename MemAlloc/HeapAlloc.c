#include <stdlib.h>

#include "HDTJMDef.h"
#include "HDTJMError.h"
#define HEAP_ALLOCATION_EXPORTS
#include "HeapAlloc.h"

// canary
#define MEM_ID		0x1d4a11

// The size of the buffer must be an aligned size.
static pHeap_t MainHeap = NULL;

pHeap_t __fastcall InitSubMem(void* buf, size_t size)
{
	pHeap_t result;
	pMemBlock_t block;
	size = GetAlignedSize(size);
	if (buf == NULL) { return NULL; }
	result = buf;
	result->Size = size;

	result->BlockList.Next = result->BlockList.Prev
		= block = (pMemBlock_t)((byte_t*)result + sizeof(Heap_t));

	result->BlockList.User = (void*)result;
	result->BlockList.bIsCache = False;
	result->Rover = block;

	block->Prev = block->Next = &result->BlockList;

	block->User = NULL;
	block->Size = result->Size - sizeof(Heap_t);

	return result;
}

pHeap_t __fastcall InitMem(size_t heapSize)
{
	const size_t size = GetAlignedSize(heapSize + sizeof(Heap_t));
	MainHeap = InitSubMem(malloc(size), size);
	return MainHeap;
}

void __fastcall ClearHeap(pHeap_t heap)
{
	pMemBlock_t block;

	heap->BlockList.Next = heap->BlockList.Prev
		= block = (pMemBlock_t)((byte_t*)heap + sizeof(Heap_t));
	heap->BlockList.User = (void*)heap;
	heap->BlockList.bIsCache = False;
	heap->Rover = block;

	block->Prev = block->Next = &heap->BlockList;

	block->User = NULL;
	block->Size = heap->Size - sizeof(Heap_t);
}

void __fastcall HFree(pHeap_t heap, void* ptr)
{
	pMemBlock_t block;
	pMemBlock_t other;

	block = (pMemBlock_t)((byte_t*)ptr - sizeof(MemBlock_t));

	Assert(block->ID == MEM_ID, "HFree: Invalid memory release.\n");

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
void* __fastcall HAlloc(pHeap_t heap, size_t size, boolean_t bCache, void* user)
{
	size_t extra;
	pMemBlock_t start;
	pMemBlock_t rover;
	pMemBlock_t newBlock;
	pMemBlock_t base;
	boolean_t bFirst = True;

	size = GetAlignedSize(size) + sizeof(MemBlock_t);

	base = heap->Rover;

	if (!base->Prev->User) { base = base->Prev; }

	rover = start = base;

	do
	{
		if (bFirst == False && rover == start)
		{
			Assert(False,
				"MyAlloc: Memory Mozarda");
			return NULL;
		}
		bFirst = False;

		if (rover->User)
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
		newBlock = (pMemBlock_t)((byte_t*)base + size);
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
			printf("Malloc: an owner is required for cache blocks\n");
			return NULL;
		}
		base->User = (void*)2;
	}
	base->bIsCache = bCache;

	heap->Rover = base->Next;

	base->ID = MEM_ID;
	return (byte_t*)base + sizeof(MemBlock_t);
}


void __fastcall DumpHeap(pHeap_t heapOrNULL)
{
	MemBlock_t* block;
	if (heapOrNULL == NULL || heapOrNULL == MainHeap)
	{
		heapOrNULL = MainHeap;
		printf("Using MainHeap\n");
	}

	printf("heapOrNULL size: %llu\tlocation: %p\n", heapOrNULL->Size, heapOrNULL);

	for (block = heapOrNULL->BlockList.Next; ; block = block->Next)
	{
		printf("block: %p\tsize:%8llu\tuser: %p\tbIsCache: %c\n",
			block, block->Size, block->User,
			0x46 + 0xe * block->bIsCache);

		if (block->Next == &heapOrNULL->BlockList)
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

void __fastcall DumpHeapFile(FILE* fp, pHeap_t heapOrNULL)
{
	MemBlock_t* block;
	if (heapOrNULL == NULL || heapOrNULL == MainHeap)
	{
		heapOrNULL = MainHeap;
		fprintf(fp, "Using MainHeap\n");
	}

	fprintf(fp, "heapOrNULL size: %llu\tlocation: %p\n", heapOrNULL->Size, heapOrNULL);

	for (block = heapOrNULL->BlockList.Next; ; block = block->Next)
	{
		fprintf(fp, "block: %p\tsize:%8llu\tuser: %p\tbIsCache: %c\n",
			block, block->Size, block->User,
			0x46 + 0xe * block->bIsCache);

		if (block->Next == &heapOrNULL->BlockList)
		{
			break;
		}

		if ((byte_t*)block + block->Size != (byte_t*)block->Next)
		{
			fprintf(fp, "Error: block size does not touch the next block\n");
		}
		if (block->Next->Prev != block)
		{
			fprintf(fp, "Error: next block doesn't have proper black link\n");
		}
		if (!block->User && !block->Next->User)
		{
			fprintf(fp, "Error: two consecutive free blocks\n");
		}
	}
}

void __fastcall CheckHeap(pHeap_t heapOrNULL)
{
	pMemBlock_t block;
	if (!heapOrNULL) { heapOrNULL = MainHeap; }
	for (block = heapOrNULL->BlockList.Next; ; block = block->Next)
	{
		if (block->Next == &heapOrNULL->BlockList)
		{
			break;
		}
		Assert((byte_t*)block + block->Size == (byte_t*)block->Next, 
			"CheckHeap: block size does not touch the next block\n");
		Assert(block->Next->Prev == block,
			"CheckHeap: two consecutive free blocks\n");
	}
}

void __fastcall ReleaseMainMem(void)
{
	ClearHeap(MainHeap);
	free(MainHeap);
}

size_t __fastcall GetFreeMemSize(pHeap_t heapOrNULL)
{
	pMemBlock_t block;
	size_t result;

	if (heapOrNULL == NULL) { heapOrNULL = MainHeap; }
	result = 0;

	for (block = heapOrNULL->BlockList.Next;
		block != &heapOrNULL->BlockList;
		block = block->Next)
	{
		if (!block->User || block->bIsCache)
		{
			result += block->Size;
		}
	}

	return result;
}

// example
//int main(void)
//{
//	pHeap_t mem = InitMem((0x80 + sizeof(MemBlock_t)) * 5);
//
//	void* m0 = HAlloc(mem, 0x80, False, NULL);
//	void* m1 = HAlloc(mem, 0x80, False, NULL);
//	void* m2 = HAlloc(mem, 0x80, True, &m2);
//	void* m3 = HAlloc(mem, 0x80, False, NULL);
//	DumpHeap(mem);
//	printf("Free space: %llu\n", GetFreeSpace(mem));
//	HFree(mem, m1);
//	DumpHeap(mem);
//	void* m4 = HAlloc(mem, 0x90, False, NULL);
//	DumpHeap(mem);
//
//	printf("Free Space: %llu\n", GetFreeSpace(mem));
//	ReleaseMainMem();
//	return 0;
//}
//
