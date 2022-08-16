#include <stdlib.h>
#include <stdio.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"

#include "Z_ZoneCopy.c"

#define ID_FOR_ALL 0x1d4a11

typedef struct MemBlock_s
{
	size_t	Size;
	void**	User;
	int		Tag;
	int		ID;
	struct MemBlock_s* Next, *Prev;
} MemBlock_t, * pMemBlock_t;

typedef struct
{
	size_t Size;
	MemBlock_t BlockList;
	MemBlock_t* Rover;
} MemAllocator_t;

MemAllocator_t* MainHeap;

MemBlock_t* InitMem(size_t heapSize)
{
	MemBlock_t*		block;
	const size_t	size = GetAlignedSize(heapSize);

	MainHeap = malloc(size);
	if (!MainHeap)
	{
		return NULL;
	}
	MainHeap->Size = size;

	MainHeap->BlockList.Next =
		MainHeap->BlockList.Prev =
		block = ((byte_t*)MainHeap + sizeof(MemAllocator_t));
	block->Prev = block->Next = &MainHeap->BlockList;
	block->User = NULL;

	block->Size = MainHeap->Size - sizeof(MemAllocator_t);
}



int main(void)
{

}