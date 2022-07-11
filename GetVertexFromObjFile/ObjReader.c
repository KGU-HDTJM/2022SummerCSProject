#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "ObjReader.h"

#define GET_VERTEX_LENGTH_BUFFER_SIZE 0x80
#pragma warning(disable : 6031)

size_t GetVertexLength(FILE* objFile)
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

float* GetVertexFromObjFileMalloc(const char* objFile, size_t* length)
{
	FILE* fp = fopen(objFile, "r");
	if (fp == NULL) { return NULL; };
	size_t vertexCount = GetVertexLength(fp);
	float* result = malloc(sizeof(float) * vertexCount);
	if (result == NULL) { goto RET_GET_VERTEX; };
	char buf[4];

	for (size_t i = 0; i < vertexCount; i += 3)
	{
		fscanf(fp, "%s %f %f %f", buf, result + i, result + i + 1, result + i + 2);
	}

RET_GET_VERTEX:
	*length = vertexCount;
	fclose(fp);
	return result;
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
