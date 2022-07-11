#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "StackAlloc.h"
#include "ObjReader.h"

int main(void)
{
	size_t vertexCount = 0;
	float* vertexArr;
	vertexArr = GetVertexFromObjFileMalloc("../resource/tetris_il_ja.obj", &vertexCount);
	printf("%p\n", vertexArr);
	printf("%llu\n", vertexCount);
	ReleaseObjVertex(vertexArr);
	
	return 0;
}