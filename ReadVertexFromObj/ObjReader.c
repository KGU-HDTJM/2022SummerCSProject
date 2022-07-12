#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "ObjReader.h"

#define GET_VERTEX_LENGTH_BUFFER_SIZE 0x80
#pragma warning(disable : 6031; disable : 4273)

static size_t __cdecl GetVertexLength(FILE* objFile)
{
	char buf[GET_VERTEX_LENGTH_BUFFER_SIZE] = { 0, };
	size_t vertexCount = 0;
	fpos_t pos = 0;

	while (fgets(buf, GET_VERTEX_LENGTH_BUFFER_SIZE, objFile) != NULL)
	{
		buf[7] = 0;
		if (*(long long*)buf == *(long long*)"# verts")
		{
			fgetpos(objFile, &pos);
			while (fgets(buf, GET_VERTEX_LENGTH_BUFFER_SIZE, objFile) != NULL)
			{
				if (buf[0] == 'v')
				{
					vertexCount++;
				}
				else if (buf[0] == '#')
				{
					goto RET_GET_VERTEX_LENGTH;
				}
			}
		}
	}

RET_GET_VERTEX_LENGTH:
	fsetpos(objFile, &pos);

	return vertexCount;
}

const float* __cdecl GetVertexFromObjFileMalloc(const char* objFile, size_t* polygonLength)
{
	FILE* fp = fopen(objFile, "r");
	if (fp == NULL) { return NULL; };
	size_t vertexCount = GetVertexLength(fp);
	float* result = malloc(sizeof(float) * 3 * vertexCount);
	if (result == NULL) { goto RET_GET_VERTEX; };
	char buf[0x80];
	size_t polygons = vertexCount / 3 - 1;
	for (size_t i = 0; i < polygons; i++)
	{
		fscanf(fp, "%s %f %f %f", buf, result + 3 * i, result + 3 * i + 1, result + 3 * i + 2);
	}

RET_GET_VERTEX:
	*polygonLength = vertexCount / 3;
	fclose(fp);
	return result;
}

void __cdecl ReleaseObjVertex(float* vertex)
{
	free(vertex);
}

/*
int main(void)
{
	const char* objFile = "../resource/tetris_il_ja.obj";

	float* vertexArr = GetVertexFromObjFileMalloc(objFile);

	if (vertexArr == NULL)
	{
		printf("fail Get vertex arr\n");
		return -1;
	}

	for (size_t i = 0; i < 9; i++)
	{
		printf("%f\t%f\t%f\n", vertexArr[i], vertexArr[i + 1], vertexArr[i + 2]);
	}

	free(vertexArr);
	return 0;
}
*/
