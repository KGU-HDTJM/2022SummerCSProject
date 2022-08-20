#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "HDTJMType.h"
#include "HDTJMDef.h"
#include "LinearAlgebra.h"

/*------i_system.c--------*/

size_t MbUsed = 6;

byte_t* IZoneBase(size_t* size)
{
	*size = MbUsed * 1024 * 1024;
	return malloc(*size);
}

void I_Error(char* error, ...)
{
	va_list	argptr;

	// Message first.
	va_start(argptr, error);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, error, argptr);
	fprintf(stderr, "\n");
	va_end(argptr);

	fflush(stderr);

	// Shutdown. Here might be other errors.
	/*if (demorecording)
		G_CheckDemoStatus();

	D_QuitNetGame();
	I_ShutdownGraphics();*/

	exit(-1);
}
/*-----/i_system.c--------*/


// z_zone.h

#define PU_STATIC		1
#define PU_SOUND		2
#define PU_MUSIC		3
#define PU_DAVE			4
#define PU_LEVEL		50
#define PU_LEVSPEC		51
#define PU_PURGELEVEL	100
#define PU_CACHE		101

void ZInit(void);
void* ZMalloc(size_t size, int tag, void* user);
void ZFree(void* ptr);
void ZFreeTags(int lowTag, int highTag);
void ZDumpHeap(int lowTag, int highTag);
void ZFileDumpHeap(FILE* fp);
void ZCheckHeap(void);
void ZChangeTag2(void* ptr, int tag);
int	ZFreeMemory(void);

typedef struct Z_MemBlock_s
{
	size_t Size;
	void** User;
	int Tag;
	int ID;
	struct Z_MemBlock_s* Next, * Prev;
} Z_MemBlock_t;

#define ZChangeTag(p, t) \
{ \
	if (( (Z_MemBlock_t*)( (byte_t*)(p) - sizeof(Z_MemBlock_t)))->ID != 0x1d4a11)\
	{\
		I_Error("Z_CT at "__FILE__":%i",__LINE__); \
	}\
	ZChangeTag2(p, t);\
}

// z_zone.c
static const char rcsid[] = "$Id: z_zone.c,v 1.4 1997/02/03 16:47:58 b1 Exp $";

#define ZONE_ID 0x1d4a11

typedef struct
{
	size_t Size;
	Z_MemBlock_t BlockList;
	Z_MemBlock_t* Rover;
} MemZone_t;

MemZone_t* MainZone;

void ZClearZone(MemZone_t* zone)
{
	Z_MemBlock_t* block;

	zone->BlockList.Next =
		zone->BlockList.Prev =  
		block = (Z_MemBlock_t*)((byte_t*)zone + sizeof(MemZone_t));

	zone->BlockList.User = (void*)zone;
	zone->BlockList.Tag = PU_STATIC;
	zone->Rover = block;

	block->Prev = block->Next = &zone->BlockList;

	block->User = NULL;
	block->Size = zone->Size - sizeof(MemZone_t);
}

void ZInit(void)
{
	Z_MemBlock_t* block;
	size_t		size;

	MainZone = /*(MemZone_t*)*/IZoneBase(&size);
	MainZone->Size = size;

	MainZone->BlockList.Next 
		= MainZone->BlockList.Prev 
		= block = /*(Z_MemBlock_t*)*/((byte_t*)MainZone + sizeof(MemZone_t));

	MainZone->BlockList.User = MainZone;
	MainZone->BlockList.Tag = PU_STATIC;
	MainZone->Rover = block;

	block->Prev = block->Next = &MainZone->BlockList;

	block->User = NULL;

	block->Size = MainZone->Size - sizeof(MemZone_t);
}

void ZFree(void* ptr)
{
	Z_MemBlock_t* block;
	Z_MemBlock_t* other;

	block = /*(Z_MemBlock_t*)*/((byte_t*)ptr - sizeof(Z_MemBlock_t));

	if (block->ID != ZONE_ID)
	{
		I_Error("Z_Free: freed a pointer without ZONE_ID");
	}

	if (IsPointer(block->User))
	{
		// smaller vules are not pointers
		// OS-depnded
		*block->User = NULL;
	}

	block->User = NULL;
	block->Tag = 0;
	block->ID = 0;

	other = block->Prev;

	if (!other->User)
	{
		// Merge with previous free blocks
		other->Size += block->Size;
		other->Next = block->Next;
		other->Next->Prev = other;

		if (block == MainZone->Rover) { MainZone->Rover = other; }

		block = other;
	}

	other = block->Next;
	if (!other->User)
	{
		block->Size += other->Size;
		block->Next = other->Next;
		block->Next->Prev = block;

		if (other == MainZone->Rover) { MainZone->Rover = block; }
	}
}

//
// Z_Malloc
// You can pass a NULL user if the  tag is < PU_PURGELEVEL.
//
#define MINFRAGMENT 64

void* ZMalloc(size_t size, int tag, void* user)
{
	size_t extra;
	Z_MemBlock_t* start;
	Z_MemBlock_t* rover;
	Z_MemBlock_t* newBlock;
	Z_MemBlock_t* base;

	size = GetAlignedSize(size);

	// scan through the block list,
	// looking for the first free block
	// of sufficient size,
	// throwing out any purgable blocks along the way.

	// account for size of block header
	size += sizeof(Z_MemBlock_t);

	// if there is a free block behind the rover,
	//	back up over them
	base = MainZone->Rover;

	if (!base->Prev->User) { base = base->Prev; }

	rover = base;
	start = base->Prev;

	do
	{
		if (rover == start)
		{
			// Scanned all the way around the list
			I_Error("Z_Malloc: failed on allocation of %i bytes", size);
		}

		if (rover->User)
		{
			if (rover->Tag < PU_PURGELEVEL)
			{
				// hit a block that can't be purged,
				//	so move base past it
				base = rover = rover->Next;
			}
			else
			{
				// free the rover block (adding the size to base)

				// the rover can be the base block
				base = base->Prev;
				ZFree((byte_t*)rover + sizeof(Z_MemBlock_t));
				base = base->Next;
				rover = base->Next;
			}
		}
		else
		{
			rover = rover->Next;
		}
	} while (base->User || base->Size < size);

	// found a block big enough
	extra = base->Size - size;
	if (extra > MINFRAGMENT)
	{
		// there will be a free fragment after the allocated block
		newBlock = ((byte_t*)base + size);
		newBlock->Size = extra;

		// NULL indicates free block.
		newBlock->User = NULL;
		newBlock->Tag = 0;
		newBlock->Prev = base;
		newBlock->Next = base->Next;
		newBlock->Next->Prev = newBlock;

		base->Next = newBlock;
		base->Size = size;
	}

	if (user)
	{
		// mark as an in use block
		base->User = user;
		*(void**)user = (void*)((byte_t*)base + sizeof(Z_MemBlock_t));
	}
	else
	{
		if (tag >= PU_PURGELEVEL)
		{
			I_Error("Z_Malloc: an owner is required for purgable blocks");
		}
		// mark as in use, but unowned
		base->User = (void*)2;
	}
	base->Tag = tag;

	// next allocation will start looking here
	MainZone->Rover = base->Next;

	base->ID = ZONE_ID;

	return (void*)((byte_t*)base + sizeof(Z_MemBlock_t));
}

void ZFreeTags(int lowTag, int highTag)
{
	Z_MemBlock_t* block;
	Z_MemBlock_t* next;

	for (block = MainZone->BlockList.Next;
		block != &MainZone->BlockList;
		block = next)
	{
		// get link before freeing
		next = block->Next;

		// free block?
		if (!block->User) { continue; }
		if (block->Tag >= lowTag && block->Tag <= highTag)
		{
			ZFree((byte_t*)block + sizeof(Z_MemBlock_t));
		}
	}
}

//
// Z_DumpHeap
// Note: TFileDumpHeap( stdout )?
//
void ZDumpHeap(int lowTag, int highTag)
{
	Z_MemBlock_t* block;

	printf("Zone size: %llu location: %p\n", MainZone->Size, MainZone);
	printf("Tag range: %d to %d", lowTag, highTag);

	for (block = MainZone->BlockList.Next; ; block = block->Next)
	{
		if (block->Tag >= lowTag && block->Tag <= highTag)
		{
			printf("block: %p\tsize:%7lld\tuser:%p\ttag:%3d\n",
				block, block->Size, block->User, block->Tag);
		}
		if (block->Next == &MainZone->BlockList)
		{
			// all blocks have been hit
			break;
		}

		if ((byte_t*)block + block->Size != (byte_t*)block->Next)
		{
			printf("ERROR: block size does not touch the next block\n");
		}
		if (block->Next->Prev != block);
		{
			printf("ERROR: next block doesn't have proper back link\n");
		}
		if (!block->User && !block->Next->User)
		{
			printf("ERROR: tow consecutive free blocks\n");
		}
	}
}

//
// ZFileDumpHeap
//
void ZFileDumpHeap(FILE* fp)
{
	Z_MemBlock_t* block;

	fprintf(fp, "zone size: %llu\tlocation: %p\n", 
		MainZone->Size, MainZone);

	for (block = MainZone->BlockList.Next; ; block = block->Next)
	{
		fprintf(fp, "block:%p\tsize:%7llu\tuser:%p\ttag:%3d\n",
			block, block->Size, block->User, block->Tag);

		if (block->Next == &MainZone->BlockList)
		{
			// all blocks have been hit
			break;
		}

		if ((byte_t*)block + block->Size != (byte_t*)block->Next)
		{
			fprintf(fp, "ERROR: block size does not touch the next block\n");
		}
		if (block->Next->Prev != block)
		{
			fprintf(fp, "ERROR: next block doesn't have proper back link\n");
		}
		if (!block->User && !block->Next->User)
		{
			fprintf(fp, "ERROR: two consecutive free blocks\n");
		}
	}
}

void ZCheckHeap(void)
{
	Z_MemBlock_t* block;

	for (block = MainZone->BlockList.Next; ; block = block->Next)
	{
		if (block->Next == &MainZone->BlockList)
		{
			// all blocks have been hit
			break;
		}
		if ((byte_t*)block + block->Size != (byte_t*)block->Next)
		{
			I_Error("Z_CheckHeap: block size does not touch the next block\n");
		}
		if (block->Next->Prev != block)
		{
			I_Error("Z_CheckHeap: two consecutive free blocks\n");
		}
	}
}


//
// Z_ChangeTag2
//
void ZChangeTag2(void* ptr, int tag)
{
	Z_MemBlock_t* block;

	block = (Z_MemBlock_t*)((byte_t*)ptr - sizeof(Z_MemBlock_t));

	if (block->ID != ZONE_ID)
	{
		I_Error("Z_ChangeTag: freed a pointer without ZONEID");
	}


	if (tag >= PU_PURGELEVEL && !IsPointer(block->User))
	{
		I_Error("Z_ChangeTag: an owner is required for purgable blocks");
	}

	block->Tag = tag;
}

//
// ZFreeMemory
//
int ZFreeMemory(void)
{
	Z_MemBlock_t* block;
	size_t		free;

	free = 0;

	for (block = MainZone->BlockList.Next;
		block != &MainZone->BlockList;
		block = block->Next)
	{
		if (!block->User || block->Tag >= PU_PURGELEVEL)
		{
			free += block->Size;
		}
	}

	return free;
}
// main.c

//int main(size_t argc, char** argv)
//{
//	size_t temp = 480 * 1024;
//	// float arr[4] = { 3.14F, 1.59F, 9.65F, 3.58F };
//	ZInit();
//	ZClearZone(MainZone);
//	ZMalloc(temp, PU_STATIC, NULL);
//	Vector4f_t* testVector = ZMalloc(temp, PU_STATIC, NULL);
//	ZDumpHeap(PU_STATIC, PU_STATIC);
//	{
//		float* testFloatVector = ZMalloc(temp, PU_STATIC, NULL);
//		for (size_t i = 0; i < temp; i++)
//		{
//			testFloatVector[i] = 3.412F * i;
//		}
//		ZDumpHeap(PU_STATIC, PU_STATIC);
//		ZFreeTags(PU_STATIC, PU_STATIC);
//		ZFree(testFloatVector);
//	}
//	printf("%d", ZFreeMemory());
//	ZFree(testVector);
//	ZDumpHeap(0, 0);
//	
//
//	free(MainZone);
//	return 0;
//}
