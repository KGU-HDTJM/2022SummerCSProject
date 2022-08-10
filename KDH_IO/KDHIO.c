#pragma warning (disable : 6031)
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HDTJMType.h"

#define BUFFER_SIZE 0x80
#define GetRGBA(red, green, blue, alpha)\
	(RGBA_t){ (byte_t)red, (byte_t)green, (byte_t)blue, (byte_t)alpha };

typedef union
{
	struct
	{
		byte_t Red;
		byte_t Green;
		byte_t Blue;
		byte_t Alpha;
	};
	int rgba;
} RGBA_t;

char buf[BUFFER_SIZE] = { 0, };

void StoreVertex(
	Vector3f_t* outVertex, FILE* fp, int* colorIndex,
	RGBA_t* colorArr, int* vertexIndexArr, int elementVertex
)
{
	float x = 0;
	float y = 0;
	float z = 0;
	int tempR = 0;
	int tempG = 0;
	int tempB = 0;
	RGBA_t tempColor = { 0, };
	for (size_t i = 0, j = 0, k = 0; i < elementVertex; i++)
	{
		fgets(buf, BUFFER_SIZE, fp);
		sscanf(buf, "%f %f %f %d %d %d", &x, &y, &z, &tempR, &tempG, &tempB);
		/*	tempColor.Red = tempR;
		tempColor.Green = tempG;
		tempColor.Blue = tempB;*/
		tempColor = GetRGBA(tempR, tempG, tempB, 0);

		// CMP_COLOR: // color_count == voxel_count; 
		for (j = 0; j < *colorIndex; j++) // 2(1)
		{
			if (colorArr[j].rgba == tempColor.rgba)
			{
				goto CMP_ELEMENTS;
			}
		}
		colorArr[*colorIndex].rgba = tempColor.rgba;
		(*colorIndex)++; // 1(0 -> 1), 
	CMP_ELEMENTS:
		for (k = 0; k < vertexIndexArr[j]; k++) // 0
		{
			if (outVertex[8 * j + k].X == x
				&& outVertex[8 * j + k].Y == y
				&& outVertex[8 * j + k].Z == z)
			{
				goto ELEMENT_EXIST;
			}
		}
		outVertex[8 * j + k].X = x;
		outVertex[8 * j + k].Y = y;
		outVertex[8 * j + k].Z = z;
		vertexIndexArr[j]++; // 1(0->1);
	ELEMENT_EXIST:;
	}
}

int main(void)
{
	int ret = 0;

	int lineCounter = 0;
	int elementVertex = 0; //반복할 횟수 저장할 곳 == aaa파일에서 72
	int colorIndex = 0;
	Vector3f_t* xyz = NULL;
	RGBA_t* colorArr = NULL;
	int* vertexIndex = NULL;
	RGBA_t tempColor = { 0, };

	char element[9];
	char vertex[7];



	FILE* fp = fopen("../resource/testio.ply", "r"); //파일 오픈
	if (fp == NULL) //파일없으면 에러처리
	{
		printf("파일 없다.\n");
		return -1;
	}

	for (lineCounter = 0; lineCounter < 4; lineCounter++)
	{
		if (NULL == fgets(buf, BUFFER_SIZE, fp))
		{
			return -1;
		}
	}

	sscanf(buf, "%s %s %d", element, vertex, &elementVertex);
	printf("%s\n", buf); //불러온거 확인용
	printf("%d\n", elementVertex);

	xyz = malloc(elementVertex * sizeof(Vector3f_t));
	colorArr = calloc(elementVertex / 8, sizeof(RGBA_t));
	vertexIndex = calloc(elementVertex / 8, sizeof(int));

	for (; lineCounter < 13; lineCounter++)
	{
		if (NULL == fgets(buf, BUFFER_SIZE, fp))
		{
			return -1;
			goto PROC_END;
		}
	}
	printf("%s\n", buf);
	StoreVertex(xyz, fp, &colorIndex, colorArr, vertexIndex, elementVertex);

	// --- file in end ---
	int vertexCount = 0;

	for (size_t i = 0; i < colorIndex; i++)
	{
		printf("\nvoxel%llu\n", i);
		for (size_t j = 0; j < 8; j++)
		{
			printf("    %f %f %f\n", xyz[8 * i + j].X, xyz[8 * i + j].Y, xyz[8 * i + j].Z);
		}
	}
	//printf("\nVertex count : % d\nVoxel count : % d\n", vertexCount, colorIndex);

PROC_END:
	free(vertexIndex);
	free(colorArr);
	free(xyz);

	fclose(fp);//파일 닫기

	return ret;
}