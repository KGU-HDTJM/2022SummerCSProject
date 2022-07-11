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
	printf("%lld\n", vertexCount);
	for (size_t i = 0; i < vertexCount; i++)
	{
		printf("%f\t", vertexArr[i]);
	}
	ReleaseObjVertex(vertexArr);
	
	return 0;
}